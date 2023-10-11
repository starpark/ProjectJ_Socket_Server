import argparse
import jinja2
import MessageParser
from datetime import datetime

def main():
    arg_parser = argparse.ArgumentParser(description='PacketGenerator')
    arg_parser.add_argument('--path', type=str, default='../../Protobuf/Message.proto', help='proto path')
    arg_parser.add_argument('--recv', type=str, default='C_', help='recv convention')
    arg_parser.add_argument('--send', type=str, default='S_', help='send convention')
    
    args = arg_parser.parse_args()
    parser = MessageParser.MessageParser(1000, args.recv, args.send)
    parser.parse_message(args.path)

    file_loader = jinja2.FileSystemLoader('Templates')
    env = jinja2.Environment(loader=file_loader)

    data = datetime.today().strftime('%Y-%m-%d')
    template = env.get_template('GamePacketHandler.h')
    output = template.render(parser=parser, date=data)

    f = open('GamePacketHandler.h', 'w+')
    f.write(output)
    f.close()

    template = env.get_template('JPacketHandler.h')
    output = template.render(parser=parser, date=data)

    f = open('JPacketHandler.h', 'w+')
    f.write(output)
    f.close()

    template = env.get_template('JPacketHandler.cpp')
    output = template.render(parser=parser, date=data)

    f = open('JPacketHandler.cpp', 'w+')
    f.write(output)
    f.close()

    return

if __name__ == '__main__':
    main();