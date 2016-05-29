#include "stdafx.h"
#include "crypto.h"

#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>
#ifndef OPENSSL_NO_ENGINE
#include <openssl/engine.h>
#endif

int add_ext(X509 *cert, int nid, char *value)
{
	X509_EXTENSION *ex;
	X509V3_CTX ctx;
	/* This sets the 'context' of the extensions. */
	/* No configuration database */
	X509V3_set_ctx_nodb(&ctx);
	/* Issuer and subject certs: both the target since it is self signed,
	* no request and no CRL
	*/
	X509V3_set_ctx(&ctx, cert, cert, NULL, NULL, 0);
	ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, value);
	if (!ex)
		return 0;

	X509_add_ext(cert, ex, -1);
	X509_EXTENSION_free(ex);
	return 1;
}
/*
Taken from http://www.cryptopp.com/wiki/Security_level#Comparable_Algorithm_Strengths
bits below stength
 if bits == 2048, the strength is comparable to 112 encryption 
 if bits == 3072, the strength is comparable to 128 encryption
 if bits == 7680, the strength is comparable to 192 encryption
 if bits == 15360, the strength is comparable to 256 encryption

*/
//struct to ensure proper cleanup
class CryptoKeys {
public:
	CryptoKeys() {
		CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);
		Bio_Err = BIO_new_fp(stderr, BIO_NOCLOSE);
		x509Certificate = X509_new();
		PrivateKey = EVP_PKEY_new();
	}
	~CryptoKeys() {

		if(x509Certificate) X509_free(x509Certificate);
		if (PrivateKey) EVP_PKEY_free(PrivateKey);
		CRYPTO_cleanup_all_ex_data();

		if (Bio_Err) {
			CRYPTO_mem_leaks(Bio_Err);
			BIO_free(Bio_Err);
		}

	}
	X509* x509Certificate = nullptr;
	EVP_PKEY* PrivateKey = nullptr;
	BIO* Bio_Err = nullptr;
};
static void callback(int p, int n, void *arg)
{
	char c = 'B';

	if (p == 0) c = '.';
	if (p == 1) c = '+';
	if (p == 2) c = '*';
	if (p == 3) c = '\n';
	fputc(c, stderr);
}
bool CreateCertificate(char* savelocation, unsigned char *country, unsigned  char *companyname, unsigned  char *commonname, int bits, int Serial, int DaysValid) {
	
	CryptoKeys cry;
	if (!cry.Bio_Err || !cry.x509Certificate || !cry.PrivateKey) return false;

	auto rsa = RSA_generate_key(bits, RSA_F4, callback, NULL);

	if (!EVP_PKEY_assign_RSA(cry.PrivateKey, rsa)) return false;


	X509_set_version(cry.x509Certificate, 2);
	ASN1_INTEGER_set(X509_get_serialNumber(cry.x509Certificate), Serial);
	X509_gmtime_adj(X509_get_notBefore(cry.x509Certificate), 0);
	X509_gmtime_adj(X509_get_notAfter(cry.x509Certificate), (long)60 * 60 * 24 * DaysValid);
	X509_set_pubkey(cry.x509Certificate, cry.PrivateKey);

	auto name = X509_get_subject_name(cry.x509Certificate);
	
	/* This function creates and adds the entry, working out the
	* correct string type and performing checks on its length.
	* Normally we'd check the return value for errors...
	*/
	X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, country, -1, -1, 0);
	X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, companyname, -1, -1, 0);
	X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, commonname, -1, -1, 0);

	/* Its self signed so set the issuer name to be the same as the
	* subject.
	*/
	X509_set_issuer_name(cry.x509Certificate, name);

	/* Add various extensions: standard extensions */
	add_ext(cry.x509Certificate, NID_basic_constraints, "critical,CA:TRUE");
	add_ext(cry.x509Certificate, NID_key_usage, "critical,keyCertSign,cRLSign");

	add_ext(cry.x509Certificate, NID_subject_key_identifier, "hash");

	/* Some Netscape specific extensions */
	add_ext(cry.x509Certificate, NID_netscape_cert_type, "sslCA");

	add_ext(cry.x509Certificate, NID_netscape_comment, "example comment extension");

	if (!X509_sign(cry.x509Certificate, cry.PrivateKey, EVP_sha256()))  return false;

	
	std::string savepem = savelocation;
	if (savepem.empty()) return false;
	if (savepem.back() != '/' && savepem.back() != '\\') return false;
	else {
		savepem += '/';
	}
	auto certloc = savepem;
	savepem += "key.pem";
	FILE * f = nullptr;
	fopen_s(&f, savepem.c_str(), "wb");
	unsigned char pass[] = "rpassword_to_be_replaced";
	PEM_write_PrivateKey(
		f,                  /* write the key to the file we've opened */
		cry.PrivateKey,               /* our key from earlier */
		EVP_des_ede3_cbc(), /* default cipher for encrypting the key on disk */
		pass,       /* passphrase required for decrypting the key on disk */
		sizeof(pass),                 /* length of the passphrase string */
		NULL,               /* callback for requesting a password */
		NULL                /* data to pass to the callback */
	);
	fclose(f);
	FILE* f1 = nullptr;
	certloc += "cert.pem";
	fopen_s(&f1, certloc.c_str(), "wb");
	PEM_write_X509(
		f1,   /* write the certificate to the file we've opened */
		cry.x509Certificate /* our certificate */
	);
	fclose(f1);
	return true;

}
