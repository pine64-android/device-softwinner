
#ifndef __AWKEYMASTER_LOGAPI_INCLUDE_H_
#define __AWKEYMASTER_LOGAPI_INCLUDE_H_

#include <utils/Log.h>
#include <hardware/keymaster.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AW_LOG_OFF
#define AW_LOG
#endif

#ifndef AW_LOG_TAG
#define AW_LOG_TAG    "AW_LOG"
#endif

#ifdef AW_TRACE_ON
#define AW_TRACE
#endif

typedef enum _LOG_LEVEL
{
    AW_LOG_TRACE = 0,
    AW_LOG_INFO,
    AW_LOG_WARNING,
    AW_LOG_ERROR
} AW_LOG_LEVEL;

static const char *log_msg[] = {
       "AW_LOG_TRACE",
       "AW_LOG_INFO",
       "AW_LOG_WARNING",
       "AW_LOG_ERROR",
};

void _Aw_SCHW_Log(AW_LOG_LEVEL logLevel, const char *msg, ...)
{
    va_list argptr;
    va_start(argptr, msg);
       ALOGE("%s:(%s:%d) %s",log_msg[logLevel],__FILE__, __LINE__, __FUNCTION__);
    va_end(argptr);
}


#ifdef AW_LOG
#define Aw_SCHW_Log(a, ...)    ((void)_Aw_SCHW_Log(a, AW_LOG_TAG, __VA_ARGS__))
#else
#define Aw_SCHW_Log(a, ...)                                                \
    do {                                                                \
        if (a == AW_LOG_ERROR)                                     \
            ((void)_Aw_SCHW_Log(a, AW_LOG_TAG, __VA_ARGS__)); \
    } while (0)
#endif

#ifdef AW_TRACE
#define Func_Entry() _Aw_SCHW_Log(AW_LOG_TRACE, AW_LOG_TAG, "%s Entry , Line: %d", __FUNCTION__, __LINE__)
#define Func_Exit() _Aw_SCHW_Log(AW_LOG_TRACE, AW_LOG_TAG, "%s Exit , Line: %d", __FUNCTION__, __LINE__)
#else
#define Func_Entry() ((void *)0)
#define Func_Exit() ((void *)0)
#endif


#ifdef __cplusplus
}
#endif


#define AW_KEYMASTER_API_VERSION 0
#define AW_KEYMASTER_CRYTPO_VERSION 1

enum  keymaster_cmd_id {
    /*
     * List the commands by the hardware.
     */
    KEYMASTER_SCHW_GENERATE_KEYPAIR    = 0x00000001,
    KEYMASTER_SCHW_IMPORT_KEYPAIR      = 0x00000002,
    KEYMASTER_SCHW_GET_KEYPAIR_PUBLIC  = 0x00000003,
    KEYMASTER_SCHW_SIGN_DATA           = 0x00000004,
    KEYMASTER_SCHW_VERIFY_DATA         = 0x00000005,

};


struct aw_keymaster_cmd {
      keymaster_cmd_id                  cid;
      keymaster_keypair_t           key_type;
         void*                       key_params;
         uint8_t*                                                key_in;
         size_t                                          key_length;
};
typedef struct aw_keymaster_cmd aw_keymaster_cmd_t;


struct aw_keymaster_resp {
      keymaster_cmd_id                         cid;
         int32_t                                status;
         uint8_t**                      keyblob_bk;
         size_t*                        keyblob_Length;
};
typedef struct aw_keymaster_resp aw_keymaster_resp_t;


struct aw_sign_data_cmd {
         keymaster_cmd_id                          cid;
         void*                  sign_param;
      uint8_t*                        key_blob;
         size_t                   key_blen;
      uint8_t*                     data;
      size_t                   data_len;
};
typedef struct aw_sign_data_cmd aw_sign_data_cmd_t;


struct aw_sign_data_resp {
      keymaster_cmd_id                         cid;
         int32_t                                status;
         uint8_t**                     signed_data;
         size_t*                           signed_dlen;
};
typedef struct aw_sign_data_resp aw_sign_data_resp_t;



struct aw_verify_data_cmd {
         keymaster_cmd_id                         cid;
         void*               verify_param;
      uint8_t*                       key_blob;
         size_t                  key_blen;
      uint8_t*             signed_data;
      size_t               signed_dlen;
         uint8_t*                           signature;
         size_t                                signat_len;
};
typedef struct aw_verify_data_cmd aw_verity_data_cmd_t;

struct aw_verify_data_resp {
      keymaster_cmd_id                         cid;
         int32_t                                status;
};
typedef struct aw_verify_data_resp aw_verity_data_resp_t;

struct aw_keymaster_handle {
    void *libreq;
    int (*aw_schw_init)(const char* buf);
    int (*aw_schw_close)(void * handle);
    int (*aw_schw_send_cmd)(const void *cbuf, uint32_t clen,
                                                       const void *rbuf, uint32_t rlen);
       int (*aw_schw_abort_err)(void * handle);
};

typedef struct aw_keymaster_handle aw_schw_handle_t;




#endif
