#!/bin/bash 


TARGET_PATH=device/softwinner/common/verity/rsa_key

DM_MERGE=$TARGET_PATH/./../dm_merge
TABLE=$TARGET_PATH/table
SIGN=$TARGET_PATH/sign
RSA_KEY=$TARGET_PATH/verity_key

JAVA_TOOL=$ANDROID_HOST_OUT/framework/dumpkey.jar


openssl genrsa -out $TARGET_PATH/rsa_key.pair 2048
openssl rsa -in $TARGET_PATH/rsa_key.pair -pubout -out $TARGET_PATH/rsa.pk

openssl req -new -out $TARGET_PATH/CertReq.csr -key $TARGET_PATH/rsa_key.pair -subj "/C=NC/ST=GD/L=ZH/O=W/OU=W/CN=0"
openssl x509 -req -in $TARGET_PATH/CertReq.csr -out $TARGET_PATH/Cert.pem -signkey $TARGET_PATH/rsa_key.pair -sha256
openssl x509 -in $TARGET_PATH/Cert.pem -inform PEM -out $TARGET_PATH/Cert.der -outform DER
java -jar ${JAVA_TOOL} $TARGET_PATH/Cert.der > $TARGET_PATH/the_key

echo " Certificat key " >$TARGET_PATH/rsa_info
cat $TARGET_PATH/the_key>>$TARGET_PATH/key_info

echo "****** Dm_meger debug info ******">>$TARGET_PATH/key_info
${DM_MERGE} -c $TARGET_PATH/the_key ${RSA_KEY} -d >>$TARGET_PATH/key_info
echo "*********************************">>$TARGET_PATH/key_info

echo " RSA key format in android libmincrypt " >>$TARGET_PATH/key_info
cat ${RSA_KEY} >> $TARGET_PATH/key_info

rm -f $TARGET_PATH/CertReq.csr $TARGET_PATH/Cert.pem 

echo "Dm-Verity Rsa key ready !"
exit 0

