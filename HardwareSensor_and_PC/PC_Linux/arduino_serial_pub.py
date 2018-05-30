#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  arduino_serial_pub.py
#  
#  Copyright 2018 Muhammad Arif Jaya <hamayame@hamayame-X450CC>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  
#  

# import library
import serial.tools.list_ports
import time
import random
import json
import serial
import paho.mqtt.client as mqtt

# callback mqtt
def callback(client, userdata, msg):
	print("message= %s" % msg.payload.decode("utf-8"))

# initiate mqtt instance
mqtt_client = mqtt.Client("cruise-skt")
mqtt_client.username_pw_set("cruise-skt", "skt")

# connect to broker
mqtt_client.connect("broker.hivemq.com", 1883)
mqtt_client.on_message = callback
mqtt_client.loop_start()

PUB_ADDRESS = "/skt/ship/"

def get_data_serial(serial):
	char_rec = serial.readline()
	return char_rec[0:(len(char_rec)-2)].rstrip()
	
def find_hw():
	ports = list(serial.tools.list_ports.comports())
	
	for p in ports:
		if "QinHeng" in p[1]:
			return p[0]

def main():
	# find hw usb conn
	hw_usb_conn = find_hw()
	
	if (hw_usb_conn == None):
		print ("Tidak terdapat perangkat, mencari perangkat")
	else:
		while(True):
			hw_usb_conn = find_hw()
			
			if (hw_usb_conn != None):
				break
	
	print("Sistem terpasang pada \"" + str(hw_usb_conn) + "\"")
	time.sleep(5)
	
	# initiate serial conn to hw
	#ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=None)
	ser = serial.Serial(hw_usb_conn, 9600, timeout=None)
	
	#data_string = json.loads(get_data_serial(ser))
	
	
	# do until killed
	while True:
		# read from hw
		data_send = get_data_serial(ser)
		
		# publish data
		mqtt_client.publish(PUB_ADDRESS + "C1", data_send, qos=2)
		print data_send
		#print data_set["id_dev"]
		#time.sleep(1)
	return 0

if __name__ == '__main__':
	main()
	ser.close()
