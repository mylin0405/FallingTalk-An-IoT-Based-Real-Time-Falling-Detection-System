# DAI2.py #coding=utf-8 -- new version of Dummy Device DAI.py, modified by tsaiwn@cs.nctu.edu.tw
import time, DAN, requests, random 
import threading, sys, os # for using a Thread to read keyboard INPUT

# ServerURL = 'http://Your_server_IP_or_DomainName:9999' #with no secure connection
#  注意你用的 IoTtalk 伺服器網址或 IP  #  https://goo.gl/6jtP41
ServerURL = 'https://7.iottalk.tw/' # with SSL secure connection
# ServerURL = 'https://Your_DomainName' #with SSL connection  (IP can not be used with https)
Reg_addr = None #if None, Reg_addr = MAC address #(本來在 DAN.py 要這樣做 :-) 
# Note that Reg_addr 在以下三句會被換掉! # the mac_addr in DAN.py is NOT used
mac_addr = 'CD86ADKLFJ38' + str( random.randint(100,999 ) )  # put here for easy to modify :-)
# 若希望每次執行這程式都被認為同一個 Dummy_Device, 要把上列 mac_addr 寫死, 不要用亂數。
Reg_addr = mac_addr   # Note that the mac_addr generated in DAN.py always be the same cause using UUID !
DAN.profile['dm_name']='Dummy_Device'   # you can change this but should also add the DM in server
DAN.profile['df_list']=['Dummy_Sensor', 'Dummy_Control']   # Check IoTtalk to see what IDF/ODF the DM has
DAN.profile['d_name']= "FallingTalk_line" #+ str( random.randint(100,999 ) ) +"_"+ DAN.profile['dm_name'] # None
DAN.device_registration_with_retry(ServerURL, Reg_addr) 
print("dm_name is ", DAN.profile['dm_name']) ; print("Server is ", ServerURL);
# global gotInput, theInput, allDead    ## 主程式不必宣告 globel, 但寫了也 OK
gotInput=False
theInput="haha"
allDead=False

# teacher tsaiwn's DAI2.py 
def doRead( ):
    global gotInput, theInput, allDead
    while True:   
        while gotInput:   # 老闆還沒把資料拿走
           time.sleep(0.1)    # 小睡 下把 CPU 暫時讓給別人
           continue  # go back to while   
        try:     # 準備讀取資料, 注意程式會卡在這等 User 輸入, 所以要用 Thread
           theInput = input("Give me data: ")
        except Exception:    ##  KeyboardInterrupt:
           allDead = True
           print("\n\nDeregister " + DAN.profile['d_name'] + " !!!\n",  flush=True)
           DAN.deregister()
           sys.stdout = sys.__stdout__
           print(" Thread say Bye bye ---------------", flush=True)
           sys.exit( );   ## break  # raise   #  ?
        if theInput =='quit' or theInput == "exit":    # these are NOT data
           allDead = True
        else:
           print("Will send " + theInput, end="   , ")
           gotInput=True   # notify my master that we have data 
        if allDead: break;   # 離開 while True 這 Loop 

# LineNotify(msg version)
def lineNotifyMessage(token, msg):

    headers = {
        "Authorization": "Bearer " + token, 
        "Content-Type" : "application/x-www-form-urlencoded"
    }

    payload = {'message': msg }
    r = requests.post("https://notify-api.line.me/api/notify", headers = headers, params = payload)
    return r.status_code

# LineNotify(img version)
def lineNotify(token, msg, picURI):
    url = "https://notify-api.line.me/api/notify"
    headers = {
        "Authorization": "Bearer " + token
    }
   
    payload = {'message': msg}
    files = {'imageFile': open(picURI, 'rb')}
    r = requests.post(url, headers = headers, params = payload, files = files)
    return r.status_code

#creat a thread to do Input data from keyboard, by tsaiwn@cs.nctu.edu.tw 
threadx = threading.Thread(target=doRead)
threadx.daemon = True
threadx.start()

#Input Line token, msg, picURI
token = "2tG7GpKKOrapOiivsEhSXn4C6Pi5DcibG6SU2FDm4VA"
msg = "危險危險, 請查閱圖片"
picURI = "test.jpg"

while True:
    try:
        if(allDead): break;
    #Pull data from a device feature called "Dummy_Control"
        value1=DAN.pull('Dummy_Control')
        if value1 != None:
            print (value1[0])
            if (int(value1[0]) == 3):
               print("send to Line!!!")
               #lineNotifyMessage("2tG7GpKKOrapOiivsEhSXn4C6Pi5DcibG6SU2FDm4VA","測試測試")
               lineNotify(token, msg, picURI)            
    #Push data to a device feature called "Dummy_Sensor" 
        #value2=random.uniform(1, 10)    ## original Dummy_Device example
        if gotInput:
           # we have data in theInput
           try:
              value2=float( theInput )
           except:
              value2=0
           if(allDead): break;
           gotInput=False   # so that you can input again 
           DAN.push ('Dummy_Sensor', value2,  value2)  #  試這:  DAN.push('Dummy_Sensor', theInput) 

    except Exception as e:
        print(e)
        if str(e).find('mac_addr not found:') != -1:
            print('Reg_addr is not found. Try to re-register...')
            DAN.device_registration_with_retry(ServerURL, Reg_addr)
        else:
            print('Connection failed due to unknow reasons.')
            time.sleep(1)    
#time.sleep(0.25)
try: 
   DAN.deregister()    # 試著解除註冊
except Exception as e:
   print("===")
print("Bye ! --------------", flush=True)
sys.exit( );