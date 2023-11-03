pushd %~dp0
protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Message.proto

GenPacket.exe --path=./Message.proto --output=GamePacketHandler --recv=C_ --send=S_
GenPacket.exe --path=./Message.proto --output=ServerPacketHandler --recv=S_ --send=C_

IF ERRORLEVEL 1 PAUSE

XCOPY /Y Enum.pb.h "./Source/Server"
XCOPY /Y Enum.pb.cc "./Source/Server"
XCOPY /Y Struct.pb.h "./Source/Server"
XCOPY /Y Struct.pb.cc "./Source/Server"
XCOPY /Y Message.pb.h "./Source/Server"
XCOPY /Y Message.pb.cc "./Source/Server"
XCOPY /Y GamePacketHandler.h "./Source/Server"

XCOPY /Y Enum.pb.h "./Source/DummyClient"
XCOPY /Y Enum.pb.cc "./Source/DummyClient"
XCOPY /Y Struct.pb.h "./Source/DummyClient"
XCOPY /Y Struct.pb.cc "./Source/DummyClient"
XCOPY /Y Message.pb.h "./Source/DummyClient"
XCOPY /Y Message.pb.cc "./Source/DummyClient"
XCOPY /Y ServerPacketHandler.h "./Source/DummyClient"

XCOPY /Y Enum.pb.h "./Source/ProjectJClient/Protocol"
XCOPY /Y Enum.pb.cc "./Source/ProjectJClient/Protocol"
XCOPY /Y Struct.pb.h "./Source/ProjectJClient/Protocol"
XCOPY /Y Struct.pb.cc "./Source/ProjectJClient/Protocol"
XCOPY /Y Message.pb.h "./Source/ProjectJClient/Protocol"
XCOPY /Y Message.pb.cc "./Source/ProjectJClient/Protocol"
XCOPY /Y JPacketHandler.cpp "./Source/ProjectJClient"
XCOPY /Y JPacketHandler.h "./Source/ProjectJClient"
XCOPY /Y JPackets.h "./Source/ProjectJClient/Protocol"

DEL /Q /F *.pb.h
DEL /Q /F *pb.cc
DEL /Q /F *.h
DEL /Q /F *.cpp
PAUSE