GPRSS = \
    /yandex/cloud/ai/stt/v2/stt_service.proto \

include($${BASEDIR}/external/grpc.pri)

PROTOS = $${GPRSS} \
    /yandex/cloud/api/operation.proto \
    /yandex/cloud/operation/operation.proto

PROTOS += \
    /google/api/annotations.proto \
    /google/api/http.proto \
    /google/rpc/status.proto

include($${BASEDIR}/external/protobuf.pri)
