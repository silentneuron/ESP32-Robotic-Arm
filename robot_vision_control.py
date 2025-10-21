import cv2, math, time, serial
import numpy as np

# Arduino serial port
arduino = serial.Serial('COM5', 115200, timeout=1)  # Adjust COM port
time.sleep(2)  # wait for esp32 to reset

FRAME_WIDTH, FRAME_HEIGHT = 640, 480
LINK1, LINK2 = 100, 100
GRIPPER_OPEN, GRIPPER_CLOSED = 90, 30

def inverse_kinematics(x, y):# The Math part
    dx = x - FRAME_WIDTH//2
    dy = FRAME_HEIGHT - y
    dist = min(math.hypot(dx, dy), LINK1+LINK2-1)
    try:
        angle_elbow = math.acos((LINK1**2+LINK2**2-dist**2)/(2*LINK1*LINK2))
        angle_shoulder = math.acos((LINK1**2+dist**2-LINK2**2)/(2*LINK1*dist))
        shoulder_deg = math.degrees(math.atan2(dy,dx) - angle_shoulder)
        elbow_deg = math.degrees(angle_elbow)
    except:
        shoulder_deg, elbow_deg = 90, 90
    return int(max(0,min(180,shoulder_deg+90))), int(max(0,min(180,elbow_deg)))

def send_angles(s,e,g):
    cmd = f"{s},{e},{g}\n"
    arduino.write(cmd.encode())
    print("[SEND]", cmd.strip())
    time.sleep(0.5)

def detect_color_center(frame, lower, upper):# Colour part
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    mask = cv2.inRange(hsv, lower, upper)
    cnts,_ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if cnts:
        c = max(cnts,key=cv2.contourArea)
        if cv2.contourArea(c)>800:
            M=cv2.moments(c)
            if M["m00"]!=0:
                cx=int(M["m10"]/M["m00"])
                cy=int(M["m01"]/M["m00"])
                return (cx,cy)
    return None

cap = cv2.VideoCapture(0)
cap.set(3, FRAME_WIDTH)
cap.set(4, FRAME_HEIGHT)

targets={}
picked=set()

while True:
    ret,frame=cap.read()
    if not ret: break
    frame=cv2.flip(frame,1)

    red_center=detect_color_center(frame,(0,120,70),(10,255,255))
    if not red_center:
        red_center=detect_color_center(frame,(170,120,70),(180,255,255))
    blue_center=detect_color_center(frame,(100,150,0),(140,255,255))

    # Conditions
    if red_center:
        targets["red"]=red_center
        cv2.circle(frame,red_center,8,(0,0,255),-1)
    if blue_center:
        targets["blue"]=blue_center
        cv2.circle(frame,blue_center,8,(255,0,0),-1)

    for color,pos in targets.items():
        if color not in picked:
            x,y=pos
            print(f"\n[PROCESSING {color.upper()}] at {x},{y}")
            s,e=inverse_kinematics(x,y)
            send_angles(s,e,GRIPPER_OPEN)
            send_angles(s,e,GRIPPER_CLOSED)
            send_angles(90,90,GRIPPER_CLOSED) # neutral
            send_angles(s,e,GRIPPER_OPEN)
            picked.add(color)

    cv2.imshow("View",frame)
    if cv2.waitKey(1)&0xFF==ord('q'):
        break

cap.release()
cv2.destroyAllWindows()

