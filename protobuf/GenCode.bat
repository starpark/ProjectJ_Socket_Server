pushd %~dp0
protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Message.proto

XCOPY /Y Enum.pb.h "./Source"
XCOPY /Y Enum.pb.cc "./Source"
XCOPY /Y Struct.pb.h "./Source"
XCOPY /Y Struct.pb.cc "./Source"
XCOPY /Y Message.pb.h "./Source"
XCOPY /Y Message.pb.cc "./Source"

DEL /Q /F *.pb.h
DEL /Q /F *pb.cc

PAUSE