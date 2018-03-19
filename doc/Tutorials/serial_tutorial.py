#!/usr/bin/env python

import sys

sys.path.append("/home/garciafa/Documents/PROG/paparazzi_messages/pprzlink/lib/v2.0/python/")

import pprzlink.serial 

import pprzlink.messages_xml_map as messages_xml_map
import pprzlink.message as message
import time

import argparse

if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--file", help="path to messages.xml file")
    parser.add_argument("-c", "--class", help="message class", dest='msg_class', default='telemetry')
    parser.add_argument("-d", "--device", help="device name", dest='dev', default='/dev/ttyUSB0')
    parser.add_argument("-b", "--baudrate", help="baudrate", dest='baud', default=115200, type=int)
    parser.add_argument("-id", "--ac_id", help="aircraft id (receiver)", dest='ac_id', default=42, type=int)
    parser.add_argument("--interface_id", help="interface id (sender)", dest='id', default=0, type=int)
    args = parser.parse_args()
    messages_xml_map.parse_messages(args.file)

    serial_interface = SerialMessagesInterface(
                                (lambda s, m: sys.stdout.write("new message from %i: %s" % (s, m))), 
                                device=args.dev, 
                                baudrate=args.baud, 
                                msg_class=args.msg_class, 
                                interface_id=args.id, 
                                verbose=True
                                )

    print("Starting serial interface on %s at %i baud" % (args.dev, args.baud))
    try:
        serial_interface.start()

        # give the thread some time to properly start
        time.sleep(0.1)

        # send some datalink messages to aicraft for test
        ac_id = args.ac_id
        print("sending ping")
        ping = PprzMessage('datalink', 'PING')
        serial_interface.send(ping, 0)

        get_setting = PprzMessage('datalink', 'GET_SETTING')
        get_setting['index'] = 0
        get_setting['ac_id'] = ac_id
        serial_interface.send(get_setting, 0)

        # change setting with index 0 (usually the telemetry mode)
        set_setting = PprzMessage('datalink', 'SETTING')
        set_setting['index'] = 0
        set_setting['ac_id'] = ac_id
        set_setting['value'] = 1
        serial_interface.send(set_setting, 0)

        while serial_interface.isAlive():
            serial_interface.join(1)
    except (KeyboardInterrupt, SystemExit):
        print('Shutting down...')
        serial_interface.stop()
        exit()

