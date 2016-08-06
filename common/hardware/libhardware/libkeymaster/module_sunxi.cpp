/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <keymaster_aw.h>
#include <log_aw.h>
#include <hardware/hardware.h>
#include <hardware/keymaster.h>
#include <nativehelper/UniquePtr.h>
#include <dlfcn.h>
#include <cutils/log.h>
#include <cutils/properties.h>

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>


typedef UniquePtr<keymaster_device_t> Unique_keymaster_device_t;
static aw_schw_handle_t* aw_km_reqst = NULL;


int aw_schw_req_libsym(const void *handle)
{
       aw_schw_handle_t *schw_handle = NULL;

       if (handle == NULL) {
               ALOGE("%s: handle is NULL ptr \n!",__func__);
               return -1;
       }
       schw_handle = (aw_schw_handle_t *)handle;

    schw_handle->libreq = dlopen("libsunxi_crypto.so", RTLD_NOW);
    if (schw_handle->libreq) {

        *(void **)(&schw_handle->aw_schw_init) =
                               dlsym(schw_handle->libreq,"AWSCHW_load_init");
        if (schw_handle->aw_schw_init == NULL) {
               ALOGE("%s:dlsym: Error Loading aw_schw_init!:%s \n",__func__,dlerror());
                   dlclose(schw_handle->libreq);
                   schw_handle->libreq  = NULL;
                   return -1;
            }


        *(void **)(&schw_handle->aw_schw_send_cmd) =
                               dlsym(schw_handle->libreq,"AWSCHW_send_cmd");
         if (schw_handle->aw_schw_send_cmd == NULL) {
                ALOGE("%s:dlsym: Error Loading aw_schw_send_cmd!:%s\n",__func__,dlerror());
                dlclose(schw_handle->libreq );
                schw_handle->libreq  = NULL;
                return -1;
         }
    } else {
        ALOGE("%s:failed to load libaw_schw library!:%s\n",__func__,dlerror());
        return -1;
    }
    return 0;
}


static void aw_schw_malloc_free(void *ptr, void *pts)
{
       if (ptr != NULL) {
               free(ptr);
       }
       if (pts != NULL) {
               free(pts);
       }
}

int aw_schw_generate_keypair(const keymaster_device_t* dev,
        const keymaster_keypair_t key_type, const void* key_params,
        uint8_t** keyBlob, size_t* keyBlobLength)
{
       aw_keymaster_cmd_t *send_cmd = NULL;
       aw_keymaster_resp_t *resp = NULL;
       aw_schw_handle_t *aw_req_handle = NULL;
       int ret = 0;

    if (dev->context == NULL) {
        ALOGE("aw_km_generate_keypair: Context NULL Ptr Err!");
        return -1;
    }

       if (key_params == NULL) {
        ALOGE("aw_km_generate_keypair: key_params NULL Ptr Err!");
        return -1;
    }

       if (keyBlob == NULL || keyBlobLength == NULL) {
        ALOGE("output key blob or length == NULL");
        return -1;
    }

    if ((key_type != TYPE_RSA) && (key_type != TYPE_DSA) && (key_type != TYPE_EC)) {
        ALOGE("Unsupported key type %d", key_type);
        return -1;
    }

       send_cmd = (aw_keymaster_cmd_t *)malloc(sizeof(aw_keymaster_cmd_t));
       if (!send_cmd ) {
               ALOGE("send malloc fail!");
               return -1;
       }
       resp = (aw_keymaster_resp_t *)malloc(sizeof(aw_keymaster_resp_t));
       if (!resp) {
               ALOGE("send or resp malloc fail!");
               free(send_cmd);
               return -1;
       }
       aw_req_handle =(aw_schw_handle_t *)dev->context;
       send_cmd->cid = KEYMASTER_SCHW_GENERATE_KEYPAIR;
       send_cmd->key_type = key_type;
       send_cmd->key_params = (void*) key_params;
       resp->cid = KEYMASTER_SCHW_GENERATE_KEYPAIR;
       resp->status = -1;
       resp->keyblob_bk = keyBlob;
       resp->keyblob_Length = keyBlobLength;
       //ALOGE("send cmd!");
       ret = (*aw_req_handle->aw_schw_send_cmd)((void *)(send_cmd), sizeof(aw_keymaster_cmd_t),
                       (void *)(resp), sizeof(aw_keymaster_resp_t));
    if ( (ret < 0)  ||  (resp->status  < 0)) {
        ALOGE("Generate key command failed resp->status = %d ret =%d", resp->status, ret);
        goto err;
    }

err:
    aw_schw_malloc_free(send_cmd, resp);
    return ret;
}


int aw_schw_import_keypair(const keymaster_device_t* dev,
        const uint8_t* key, const size_t key_length,
        uint8_t** key_blob, size_t* key_blob_length)
{
       aw_keymaster_cmd_t *send_cmd = NULL;
       aw_keymaster_resp_t *resp    = NULL;
       aw_schw_handle_t *aw_req_handle = NULL;
       int ret = 0;

       //ALOGE("%s:: Enter\n",__func__);

    if (dev->context == NULL) {
        ALOGE("qcom_km_import_keypair: Context  == NULL");
        return -1;
    }

    if (key == NULL) {
        ALOGE("Input key == NULL");
        return -1;
    }

       if (key_blob == NULL || key_blob_length == NULL) {
        ALOGE("Output key blob or length == NULL");
        return -1;
    }
       send_cmd = (aw_keymaster_cmd_t *)malloc(sizeof(aw_keymaster_cmd_t));
       if (!send_cmd) {
               ALOGE("send malloc fail!");
               return -1;
       }
       resp = (aw_keymaster_resp_t *)malloc(sizeof(aw_keymaster_resp_t));
       if (!resp) {
               ALOGE("resp malloc fail!");
               free(send_cmd);
               return -1;
       }
       aw_req_handle =(aw_schw_handle_t *)dev->context;

       send_cmd->cid = KEYMASTER_SCHW_IMPORT_KEYPAIR;
       send_cmd->key_in = (uint8_t *)key;
       send_cmd->key_length = key_length;

       resp->cid = KEYMASTER_SCHW_IMPORT_KEYPAIR;
       resp->status = -1;
       resp->keyblob_bk = key_blob;
       resp->keyblob_Length = key_blob_length;
       ret = (*aw_req_handle->aw_schw_send_cmd)((void *)send_cmd ,sizeof(aw_keymaster_cmd_t),
                       (void *)resp ,sizeof(aw_keymaster_resp_t));
    if ((ret < 0) || (resp->status  < 0)) {
        ALOGE("import keypair command failed resp->status = %d ret =%d", resp->status, ret);
        goto err;
    }

 err:
    aw_schw_malloc_free(send_cmd, resp);
    return ret;
}



int aw_schw_get_keypair_public(const struct keymaster_device* dev,
        const uint8_t* key_blob, const size_t key_blob_length,
        uint8_t** x509_data, size_t* x509_data_length)
{
       aw_keymaster_cmd_t *send_cmd = NULL;
       aw_keymaster_resp_t *resp    = NULL;
       aw_schw_handle_t *aw_req_handle = NULL;
       int ret = 0;

       //ALOGE("%s:: Enter\n",__func__);

    if (x509_data == NULL || x509_data_length == NULL) {
        ALOGE("Output public key buffer == NULL");
        return -1;
    }

    if (x509_data == NULL) {
        ALOGE("Supplied key blob was NULL");
        return -1;
    }
       send_cmd = (aw_keymaster_cmd_t *)malloc(sizeof(aw_keymaster_cmd_t));
       if (!send_cmd) {
               ALOGE("send malloc fail!");
               return -1;
       }
       resp = (aw_keymaster_resp_t *)malloc(sizeof(aw_keymaster_resp_t));
       if (!resp) {
               ALOGE("resp malloc fail!");
               free(send_cmd);
               return -1;
       }

       aw_req_handle =(aw_schw_handle_t *)dev->context;

       send_cmd->cid = KEYMASTER_SCHW_GET_KEYPAIR_PUBLIC;
       send_cmd->key_in= (uint8_t *)key_blob;
       send_cmd->key_length = key_blob_length;

       resp->cid = KEYMASTER_SCHW_GET_KEYPAIR_PUBLIC;
       resp->status = -1;
       resp->keyblob_bk = x509_data;
       resp->keyblob_Length= x509_data_length;
       ret = (*aw_req_handle->aw_schw_send_cmd)((void *)send_cmd ,sizeof(aw_keymaster_cmd_t),
                               (void *)resp ,sizeof(aw_keymaster_resp_t));
    if ((ret < 0) || (resp->status  < 0)) {
        ALOGE("get keypair public command failed resp->status = %d ret =%d", resp->status, ret);
               goto err;
    }

err:
       aw_schw_malloc_free(send_cmd, resp);
    return ret;
}


int aw_schw_sign_data(const keymaster_device_t* dev,
        const void* params,
        const uint8_t* keyBlob, const size_t keyBlobLength,
        const uint8_t* data, const size_t dataLength,
        uint8_t** signedData, size_t* signedDataLength)
{
       aw_sign_data_cmd_t *sign_cmd = NULL;
       aw_sign_data_resp_t *sign_resp = NULL;
       aw_schw_handle_t *aw_req_handle = NULL;
       int ret = 0;
       //ALOGE("%s:: Enter\n",__func__);
       if (dev->context == NULL) {
        ALOGE("qcom_km_sign_data: Context  == NULL");
        return -1;
    }

    if (data == NULL) {
        ALOGE("input data to sign == NULL");
        return -1;
    }

       if (signedData == NULL || signedDataLength == NULL) {
        ALOGE("Output signature buffer == NULL");
        return -1;
    }

       sign_cmd = (aw_sign_data_cmd_t *)malloc(sizeof(aw_sign_data_cmd_t));
       if (!sign_cmd) {
               ALOGE("send malloc fail!");
               return -1;
       }
       sign_resp = (aw_sign_data_resp_t *)malloc(sizeof(aw_sign_data_resp_t));
       if (!sign_resp) {
               ALOGE("resp malloc fail!");
               free(sign_cmd);
               return -1;
       }

       aw_req_handle =(aw_schw_handle_t *)dev->context;
       sign_cmd->cid = KEYMASTER_SCHW_SIGN_DATA ;
       sign_cmd->key_blob = (uint8_t *)keyBlob;
       sign_cmd->key_blen = (size_t)keyBlobLength;
       sign_cmd->sign_param = (void *)params;
       sign_cmd->data = (uint8_t *)data;
       sign_cmd->data_len = (size_t)dataLength;
       sign_resp->status = -1;
       sign_resp->cid = KEYMASTER_SCHW_SIGN_DATA;
       sign_resp->signed_data = signedData;
       sign_resp->signed_dlen = signedDataLength;

       ret = (*aw_req_handle->aw_schw_send_cmd)((void *)sign_cmd ,sizeof(aw_sign_data_cmd_t),
                               (void *)sign_resp ,sizeof(aw_sign_data_resp_t));

    if ((ret < 0) || (sign_resp->status  < 0)) {
        ALOGE("Sign data command failed resp->status = %d ret =%d", sign_resp->status, ret);
        goto err;
    }
    return 0;
err:
       aw_schw_malloc_free(sign_cmd, sign_resp);
    return -1;

}



int aw_schw_verify_data(const keymaster_device_t* dev,
        const void* params,
        const uint8_t* keyBlob, const size_t keyBlobLength,
        const uint8_t* signedData, const size_t signedDataLength,
        const uint8_t* signature, const size_t signatureLength)
{
       aw_verity_data_cmd_t *verity_cmd = NULL;
       aw_verity_data_resp_t *verity_resp = NULL;
       aw_schw_handle_t *aw_req_handle = NULL;
       int ret = 0;
       //ALOGE("%s:: Enter\n",__func__);
    if (dev->context == NULL) {
        ALOGE("aw_km_verify_data: Context  == NULL");
        return -1;
    }

    if (signedData == NULL || signature == NULL) {
        ALOGE("data or signature buffers == NULL");
        return -1;
    }

    verity_cmd = (aw_verity_data_cmd_t *)malloc(sizeof(aw_verity_data_cmd_t));
       if (!verity_cmd) {
               ALOGE("send malloc fail!");
               return -1;
       }
       verity_resp = (aw_verity_data_resp_t *)malloc(sizeof(aw_verity_data_resp_t));
       if (!verity_resp) {
               ALOGE("resp malloc fail!");
               free(verity_cmd);
               return -1;
       }
       aw_req_handle =(aw_schw_handle_t *)dev->context;
       verity_cmd->cid = KEYMASTER_SCHW_VERIFY_DATA;
       verity_cmd->verify_param = (void*)params;
       verity_cmd->key_blob = (uint8_t *)keyBlob;
       verity_cmd->key_blen = (size_t)keyBlobLength;
       verity_cmd->signed_data = (uint8_t *)signedData;
       verity_cmd->signed_dlen = (size_t)signedDataLength;
       verity_cmd->signature = (uint8_t *)signature;
       verity_cmd->signat_len = (size_t)signatureLength;

       verity_resp->cid = KEYMASTER_SCHW_VERIFY_DATA;
       verity_resp->status = -1;
       ret = (*aw_req_handle->aw_schw_send_cmd)((void *)verity_cmd ,sizeof(aw_verity_data_cmd_t),
                       (void *)verity_resp ,sizeof(aw_verity_data_resp_t));

    if ( (ret < 0)  ||  (verity_resp->status  < 0)) {
        ALOGE("Verify data command failed resp->status = %d ret =%d", verity_resp->status, ret);
        goto err;
    }
    return 0;
err:
       aw_schw_malloc_free(verity_cmd, verity_resp);
    return -1;
}



/* Close an opened aw schw instance */
static int aw_device_close(hw_device_t *dev) {
       if (aw_km_reqst)
               free(aw_km_reqst);
    delete dev;
    return 0;
}

/*
 * Generic device handling
 */
static int aw_device_open(const hw_module_t* module, const char* name,
        hw_device_t** device) {

       const char *test_buf = {"Aw keymaster module get the app init!"};
       int ret = 0;

    if (strcmp(name, KEYSTORE_KEYMASTER) != 0)
        return -EINVAL;
    //ALOGE("%s:: Enter AW keymaster\n",__func__);
    aw_km_reqst = (aw_schw_handle_t *)malloc(sizeof(aw_schw_handle_t));
    if (aw_km_reqst == NULL) {
        ALOGE("Memalloc for aw keymaster handle failed");
        return -1;
    }
    aw_km_reqst->libreq= NULL;
    ret = aw_schw_req_libsym((void*)aw_km_reqst);
    if (ret < 0) {
        free(aw_km_reqst);
        return -1;
    }

    (*aw_km_reqst->aw_schw_init)(test_buf);

    Unique_keymaster_device_t dev(new keymaster_device_t);
    if (dev.get() == NULL)
        return -ENOMEM;
    dev->context = (void *)aw_km_reqst;
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 1;
    dev->common.module = (struct hw_module_t*) module;
    dev->common.close = aw_device_close;

    dev->flags = 0;

    dev->generate_keypair = aw_schw_generate_keypair;
    dev->import_keypair = aw_schw_import_keypair;
    dev->get_keypair_public = aw_schw_get_keypair_public;
    dev->delete_keypair = NULL;
    dev->delete_all = NULL;
    dev->sign_data = aw_schw_sign_data;
    dev->verify_data = aw_schw_verify_data;

    *device = reinterpret_cast<hw_device_t*>(dev.release());

    ALOGE("%s:AW keymaster open sucessfully!\n",__func__);
    return 0;
}

static struct hw_module_methods_t keystore_module_methods = {
    open: aw_device_open,
};

struct keystore_module HAL_MODULE_INFO_SYM
__attribute__ ((visibility ("default"))) = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        module_api_version: KEYMASTER_MODULE_API_VERSION_0_2,
        hal_api_version: HARDWARE_HAL_API_VERSION,
        id: KEYSTORE_HARDWARE_MODULE_ID,
        name: "Keymaster AllSoftwinnertech HAL",
        author: "The Android Open Source Project",
        methods: &keystore_module_methods,
        dso: 0,
        reserved: {},
    },
};
