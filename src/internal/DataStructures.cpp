#include "internal/DataStructures.h"
#if WIN32
#include <wincrypt.h>
#endif
namespace SL {
    namespace WS_LITE {

#if WIN32

        void add_other_root_certs(asio::ssl::context &ctx)
        {
            HCERTSTORE hStore = CertOpenSystemStore(0, "ROOT");
            if (hStore == NULL) {
                return;
            }

            X509_STORE *store = X509_STORE_new();
            PCCERT_CONTEXT pContext = NULL;
            while ((pContext = CertEnumCertificatesInStore(hStore, pContext)) != NULL) {
                // convert from DER to internal format
                X509 *x509 = d2i_X509(NULL, (const unsigned char **)&pContext->pbCertEncoded, pContext->cbCertEncoded);
                if (x509 != NULL) {
                    X509_STORE_add_cert(store, x509);
                    X509_free(x509);
                }
            }

            CertFreeCertificateContext(pContext);
            CertCloseStore(hStore, 0);

            // attach X509_STORE to boost ssl context
            SSL_CTX_set_cert_store(ctx.native_handle(), store);
        }

#else 
        void add_other_root_certs(asio::ssl::context &ctx) {}
#endif
    }
}
