#ifndef CRYPTO_HPP
#define	CRYPTO_HPP
#include <memory>
#include <string>
#include <math.h>

#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <openssl/md5.h>

namespace SL {

	namespace Remote_Access_Library {
		//type must support size(), resize() and operator[]
		namespace Crypto {
			//thiks initializes all of the openssl library..
			//the destructor ensures proper cleanup
			namespace INTERNAL {
				extern bool Started;
			}
			class Initer {
				
			public:
				
				Initer();
				~Initer();
			};
		

			struct CertInfo {
				std::string savelocation;//the folder you want to save the key and cert to
				std::string password;//this is the password used to encrypt the private key, this is good practice. It means a password is needed to read it
				std::string filename; //should only be a filename without an extension, i.e:     selfsigned

				std::string country;
				std::string companyname;
				std::string commonname;
			
				int bits = 3072;
				int Serial = 0;
				int DaysValid = 365;

			};
			struct CertSaveLocation {
				std::string Certificate;
				std::string Private_Key;
			};
			//if the strings inside CertSaveLocation are empty, the function failed
			CertSaveLocation CreateCertificate(const CertInfo& info);
			//if the strings returned is empty, the function failed!
			//THIS FUNCTION TAKES A LONG TIME TO GENERATE SO BE PATIENT.. 
			std::string Createdhparams(std::string savelocation, std::string filename, int bits = 3072);
			//below is a 3072 bit dhparm I generated. This does not need to be kept secret.. it takes some time to generate these . . . 

			std::string ValidateCertificate(const std::string& fullpathtocert);
			std::string ValidatePrivateKey(const std::string& fullpathtoprivatekey, std::string& password);

			const std::string dhparams = "-----BEGIN DH PARAMETERS-----\n"
				"MIIBCAKCAQEAzPjrDCNwq0bYz5xi72GM4EoWjqwmaAXFcY7vR6+nDpeeKpeYg8XS\n"
				"tdjCwyaIRaYO3tzZxTbjdgnCbksVQrRKscefnba5zguqnqvaGL7rHNwdqaQq7rTp\n"
				"KGdva8BKIaE9fq2q0OaZpBWE7KtVdraF5+CnvEj4AJqxGgZ/OtP+Y3UPTIcjoIve\n"
				"2ss+XbvHGvcZ+RmyYeHmmQbWyqNyoUgrMzfGOHUvY6x9fcl/DvCSIpVn8qRK+3+n\n"
				"64R3OAMcxNK7ONGSL3q6DRVTUXTNch9W+TKOYAppKtdyemoUAMleXD3F282BUwqL\n"
				"f4pf5b+c+w+99vHpUlkbIzV0tI5vGZo1uwIBAg==\n"
				"-----END DH PARAMETERS-----\n";

			namespace Base64 {
				template<class type>
				void encode(const type& ascii, type& base64) {
					BIO *bio, *b64;
					BUF_MEM *bptr;

					b64 = BIO_new(BIO_f_base64());
					BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
					bio = BIO_new(BIO_s_mem());
					BIO_push(b64, bio);
					BIO_get_mem_ptr(b64, &bptr);

					//Write directly to base64-buffer to avoid copy
					int base64_length = static_cast<int>(round(4 * ceil((double)ascii.size() / 3.0)));
					base64.resize(base64_length);
					bptr->length = 0;
					bptr->max = base64_length + 1;
					bptr->data = (char*)&base64[0];

					BIO_write(b64, &ascii[0], static_cast<int>(ascii.size()));
					BIO_flush(b64);

					//To keep &base64[0] through BIO_free_all(b64)
					bptr->length = 0;
					bptr->max = 0;
					bptr->data = nullptr;

					BIO_free_all(b64);
				}
				template<class type>
				type encode(const type& ascii) {
					type base64;
					encode(ascii, base64);
					return base64;
				}

				template<class type>
				void decode(const type& base64, type& ascii) {
					//Resize ascii, however, the size is a up to two bytes too large.
					ascii.resize((6 * base64.size()) / 8);
					BIO *b64, *bio;

					b64 = BIO_new(BIO_f_base64());
					BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
					bio = BIO_new_mem_buf((char*)&base64[0], static_cast<int>(base64.size()));
					bio = BIO_push(b64, bio);

					int decoded_length = BIO_read(bio, &ascii[0], static_cast<int>(ascii.size()));
					ascii.resize(decoded_length);

					BIO_free_all(b64);
				}
				template<class type>
				type decode(const type& base64) {
					type ascii;
					decode(base64, ascii);
					return ascii;
				}

			}

			template<class type>
			void MD5(const type& input, type& hash) {
				MD5_CTX context;
				MD5_Init(&context);
				MD5_Update(&context, &input[0], input.size());

				hash.resize(128 / 8);
				MD5_Final((unsigned char*)&hash[0], &context);
			}
			template<class type>
			type MD5(const type& input) {
				type hash;
				MD5(input, hash);
				return hash;
			}

			template<class type>
			void SHA1(const type& input, type& hash) {
				SHA_CTX context;
				SHA1_Init(&context);
				SHA1_Update(&context, &input[0], input.size());

				hash.resize(160 / 8);
				SHA1_Final((unsigned char*)&hash[0], &context);
			}
			template<class type>
			type SHA1(const type& input) {
				type hash;
				SHA1(input, hash);
				return hash;
			}

			template<class type>
			void SHA256(const type& input, type& hash) {
				SHA256_CTX context;
				SHA256_Init(&context);
				SHA256_Update(&context, &input[0], input.size());

				hash.resize(256 / 8);
				SHA256_Final((unsigned char*)&hash[0], &context);
			}
			template<class type>
			type SHA256(const type& input) {
				type hash;
				SHA256(input, hash);
				return hash;
			}

			template<class type>
			void SHA512(const type& input, type& hash) {
				SHA512_CTX context;
				SHA512_Init(&context);
				SHA512_Update(&context, &input[0], input.size());

				hash.resize(512 / 8);
				SHA512_Final((unsigned char*)&hash[0], &context);
			}
			template<class type>
			type SHA512(const type& input) {
				type hash;
				SHA512(input, hash);
				return hash;
			}
		}
	}
}
#endif	

