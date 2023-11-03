import argparse
import jinja2
import MessageParser
from datetime import datetime

def main():
    arg_parser = argparse.ArgumentParser(description='PacketGenerator')
    arg_parser.add_argument('--path', type=str, default='./Message.proto', help='proto path')
    arg_parser.add_argument('--output', type=str, default='TestPacketHandler', help='output file name')
    arg_parser.add_argument('--recv', type=str, default='C_', help='recv convention')
    arg_parser.add_argument('--send', type=str, default='S_', help='send convention')
    
    args = arg_parser.parse_args()
    parser = MessageParser.MessageParser(1000, args.recv, args.send)
    parser.parse_message(args.path)

    file_loader = jinja2.FileSystemLoader('Templates')
    env = jinja2.Environment(loader=file_loader)
    date = datetime.today().strftime('%Y-%m-%d')

    # GamePacketHandler for Server
    template = env.get_template('PacketHandler.h')
    output = template.render(parser=parser, date=date, output=args.output)
    f = open(args.output + '.h', 'w+', encoding='UTF8')
    f.write(output)
    f.close()

    if args.output == 'GamePacketHandler':
        # JPacketHandler for Client
        template = env.get_template('JPacketHandler.h')
        output = template.render(parser=parser, date=date)
        f = open('JPacketHandler.h', 'w+', encoding='UTF8')
        f.write(output)
        f.close()

        template = env.get_template('JPacketHandler.cpp')
        output = template.render(parser=parser, date=date)
        f = open('JPacketHandler.cpp', 'w+', encoding='UTF8')
        f.write(output)
        f.close()

        # JPackets for Client
        template = env.get_template('JPackets.h')
        output = template.render(parser=parser, date=date)
        f = open('JPackets.h', 'w+', encoding='UTF8')
        f.write(output)
        f.close()

        # Add line disable warning 4800
        with open('Message.pb.h', 'r+', encoding='UTF8') as file:
            file_data = file.read()
            file.seek(0, 0)
            file.write('#pragma warning(disable:4800)\n'+ file_data)

        with open('Struct.pb.h', 'r+', encoding='UTF8') as file:
            file_data = file.read()
            file.seek(0, 0)
            file.write('#pragma warning(disable:4800)\n'+ file_data)

        with open('Enum.pb.h', 'r+', encoding='UTF8') as file:
            file_data = file.read()
            file.seek(0, 0)
            file.write('#pragma warning(disable:4800)\n'+ file_data)


    return

if __name__ == '__main__':
    main();