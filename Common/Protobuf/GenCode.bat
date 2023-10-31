pushd %~dp0
protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Message.proto

GenPacket.exe --path=./Message.proto --recv=C_ --send=S_

XCOPY /Y Enum.pb.h "./Source"
XCOPY /Y Enum.pb.cc "./Source"
XCOPY /Y Struct.pb.h "./Source"
XCOPY /Y Struct.pb.cc "./Source"
XCOPY /Y Message.pb.h "./Source"
XCOPY /Y Message.pb.cc "./Source"
XCOPY /Y GamePacketHandler.h "./Source"
XCOPY /Y JPacketHandler.cpp "./Source"
XCOPY /Y JPacketHandler.h "./Source"
XCOPY /Y JPackets.h "./Source"

DEL /Q /F *.pb.h
DEL /Q /F *pb.cc
DEL /Q /F GamePacketHandler.h
DEL /Q /F JPacketHandler.cpp
DEL /Q /F JPacketHandler.h
DEL /Q /F JPackets.h

PAUSE