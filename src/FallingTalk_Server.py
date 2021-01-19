import time, random, requests
import DAN
import zmq

ServerURL = 'https://7.iottalk.tw/'      #with non-secure connection
#ServerURL = 'https://DomainName' #with SSL connection
Reg_addr = None #if None, Reg_addr = MAC address

DAN.profile['dm_name']='Dummy_Device'
DAN.profile['df_list']=['Dummy_Sensor', "Dummy_Control"]
DAN.profile['d_name']= 'FallingTalk_Server' 

DAN.device_registration_with_retry(ServerURL, Reg_addr)
#DAN.deregister()  #if you want to deregister this device, uncomment this line
#exit()            #if you want to deregister this device, uncomment this line

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://*:5555")

while True:
    try:
        message = socket.recv()
        temp = ""
        print("Received request: %s" % message) 
        msg = message.decode()     
        DAN.push ('Dummy_Sensor', int(msg)) #Push data to an input device feature "Dummy_Sensor"
        socket.send(b"OK")
        #==================================

    except Exception as e:
        print(e)
        if str(e).find('mac_addr not found:') != -1:
            print('Reg_addr is not found. Try to re-register...')
            DAN.device_registration_with_retry(ServerURL, Reg_addr)
        else:
            print('Connection failed due to unknow reasons.')
            time.sleep(1)    


