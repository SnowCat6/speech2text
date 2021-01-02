# Based on: http://code.google.com/p/ostinato/source/browse/protobuf.pri
#
# Qt qmake integration with Google Protocol Buffers compiler protoc
#
# To compile protocol buffers with qt qmake, specify PROTOS variable and
# include this file
#
# Example:
# PROTOS = a.proto b.proto
# include(protobuf.pri)
#
# Set PROTO_PATH if you need to set the protoc --proto_path search path
# Set PROTOC to the path to the protoc compiler if it is not in your $PATH
#
PKGCONFIG += protobuf gRPC

isEmpty(PROTO_DIR):PROTO_DIR = .
isEmpty(PROTOC):PROTOC = protoc

PROTO_DIR = $$shell_path($${PWD}/cloudapi)
PROTO_OUT= $$shell_path($${OUT_PWD}/protoc)

PROTO_PATH = /
PROTOPATHS =
for(p, PROTO_PATH){
    PROTOPATHS += --proto_path=$${PROTO_DIR}$${p}
    INCLUDEPATH += $${PROTO_OUT}$${p}
}

PROTOSPATH=
for(p, PROTOS):PROTOSPATH+=$${PROTO_DIR}$${p}

defineReplace(protoHeaderFile) {
    path = $$1
    path = $$replace(path, $$re_escape($$PROTO_DIR), $$PROTO_OUT)
    path = $$replace(path, $$re_escape(.proto), .pb.h)
    return($$path)
}

defineReplace(protoSourceFile) {
    path = $$1
    path = $$replace(path, $$re_escape(.pb.h), .pb.cc)
    return($$path)
}

protobuf_decl.name  = protobuf header
protobuf_decl.input = PROTOSPATH
protobuf_decl.output_function = protoHeaderFile
protobuf_decl.commands = $${PROTOC} \
    --cpp_out=$${PROTO_OUT} \
    $${PROTOPATHS} \
    --proto_path=${QMAKE_FILE_IN_PATH} \
    ${QMAKE_FILE_NAME}
protobuf_decl.variable_out = PROTO_GENERATED_FILES
QMAKE_EXTRA_COMPILERS += protobuf_decl
GENERATED_FILES += PROTO_GENERATED_FILES

protobuf_impl.name  = protobuf implementation
protobuf_impl.input = PROTO_GENERATED_FILES
protobuf_impl.output_function = protoSourceFile
protobuf_impl.commands = $$escape_expand(\\n)
protobuf_impl.variable_out = GENERATED_SOURCES
QMAKE_EXTRA_COMPILERS += protobuf_impl
