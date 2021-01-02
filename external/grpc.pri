PKGCONFIG += protobuf grpc++

isEmpty(PROTO_DIR):PROTO_DIR = .
isEmpty(PROTOC):PROTOC = protoc
isEmpty(GRPC_CPP_PLUGIN){
    BIN_FOLDER=
    isEmpty(BIN_FOLDER):BIN_FOLDER=$$(BIN_FOLDER)
    isEmpty(BIN_FOLDER):BIN_FOLDER=$$[QT_INSTALL_BINS]
    GRPC_CPP_PLUGIN=$${BIN_FOLDER}/grpc_cpp_plugin
    win32:GRPC_CPP_PLUGIN=$${GRPC_CPP_PLUGIN}.exe
}

PROTO_DIR = $$shell_path($${PWD}/cloudapi)
PROTO_OUT= $$shell_path($${OUT_PWD}/protoc)

PROTO_PATH = /
PROTOPATHS =
for(p, PROTO_PATH){
    PROTOPATHS += --proto_path=$$shell_path($${PROTO_DIR}$${p})
    INCLUDEPATH += $${PROTO_OUT}$${p}
}

GPRSSPATH=
for(p, GPRSS):GPRSSPATH+=$${PROTO_DIR}$${p}

defineReplace(protoHeaderFileGRPC) {
    path = $$1
    path = $$replace(path, $$re_escape($$PROTO_DIR), $$PROTO_OUT)
    path = $$replace(path, $$re_escape(.proto), .grpc.pb.h)
    return($$path)
}

defineReplace(GPRSSourceFileGRPC) {
    path = $$1
    path = $$replace(path, $$re_escape(.pb.h), .pb.cc)
    return($$path)
}

grpc_decl.name  = gprs header
grpc_decl.input = GPRSSPATH
grpc_decl.output_function = protoHeaderFileGRPC
grpc_decl.commands = $${PROTOC} \
    $${PROTOPATHS} \
    --cpp_out=$${PROTO_OUT} \
    --grpc_out=$${PROTO_OUT} \
    --plugin=protoc-gen-grpc=$${GRPC_CPP_PLUGIN} \
    --proto_path=${QMAKE_FILE_IN_PATH} \
    ${QMAKE_FILE_NAME}
grpc_decl.variable_out = GRPC_GENERATED_FILES
QMAKE_EXTRA_COMPILERS += grpc_decl
GENERATED_FILES += GRPC_GENERATED_FILES

grpc_impl.name  = gprs implementation
grpc_impl.input = GRPC_GENERATED_FILES
grpc_impl.output_function = GPRSSourceFileGRPC
grpc_impl.commands = $$escape_expand(\\n)
grpc_impl.variable_out = GENERATED_SOURCES
QMAKE_EXTRA_COMPILERS += grpc_impl
