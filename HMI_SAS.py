from tkinter import *
import tkinter as tk
import socket
import time
import math
import decimal
from PIL import ImageTk,Image

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 30002        # The port used by the server

# Starting TCP
global s
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
#s.sendall(b'Hello, world')
#data = s.recv(1024)
#print('Received', repr(data.decode("utf-8")))

# ========================== TCP variables ==========================
# RehaMove3_Req_Type
global Move3_none, Move3_incr, Move3_decr, Move3_ramp_more, Move3_ramp_less, Move3_stop, Move3_start, Move3_done
Move3_none = 0
Move3_incr = 1
Move3_decr = 2
Move3_ramp_more = 3
Move3_ramp_less = 4
Move3_stop = 5
Move3_start = 6
Move3_done = 7
# RehaIngest_Req_Type
global Inge_none
Inge_none = 0
# User_Req_Type
global User_none, User_CM, User_CA, User_X, User_th, User_st
User_none = 0
User_CM = 1
User_CA = 2
User_X = 3
User_th = 4
User_st = 5
# Initialize
global stim_code, rec_code, rob_status, rep_nr, user_code
rob_status = 'R'
rep_nr = 20
stim_code = Move3_none
rec_code = Inge_none
user_code = User_none
global counters
# ========================== Internal variables ==========================
global max_rep, min_rep, max_cur, min_curr, max_ramp, min_ramp
global Drep, Dcur, Dramp, i_rep, i_cur, i_ramp, init_cur, init_ramp, init_rep
max_rep = 99
min_rep = 1
max_cur = 5.0
min_cur = 0.0
max_ramp = 3
min_ramp = 1
# Increment and initial values
Drep = 1
Dcur = 1
Dramp = 1
init_ramp = 3
init_cur = 15
init_rep = 20
# This is just some index value
i_rep = 3
i_cur = 2
i_ramp = 1

# Colours
light_red = '#D95319'
dark_red = '#A2142F'
standard_green = '#77AC30'
standard_blue = '#0052cc'
goldy = '#EDB120'
purply = '#7E2F8E'
lsr_text = '#15568a'
lsr_line = '#25a9d1'
lsr_plus = '#0da140'
aau_blue = '#211a52'
# ========================== Window ==========================
root = tk.Tk()
root.title("SAS interface")
root.geometry("900x500")

def TCP_update():
    global stim_code, rec_code, rob_status, rep_nr
    # Encode message
    message = 'SCREEN;'+ str(stim_code) + ';' + str(user_code) + ';' + rob_status + str(rep_nr) + ';'
    #print('Sending...')
    s.sendall(message.encode("utf-8"))
    # Decode message
    #print('Receiving...')
    data = s.recv(516)
    #print('Received', repr(data.decode("utf-8")))
    #data_message = data.decode("utf-8")
    data_ls = (data.decode("utf-8")).split(";")
    #print(data_ls)
    if(data_ls[0]=='SAS'):
        #print('Beginning is goood')
        temp_cur = float(data_ls[1])
        temp_ramp = float(data_ls[2])
        global i_ramp, i_cur, counters
        counters[i_ramp].set(temp_ramp)
        counters[i_cur].set(temp_cur)

def increase_stat(event=None, counter=None, incr=None, limit=None, type=None):
    global stim_code, rep_nr, user_code
    user_code = User_none

    if (type == i_ramp):
        stim_code = Move3_ramp_more
        #print('Move3_ramp_more')
    elif (type == i_cur):
        stim_code = Move3_incr
        #print('Move3_incr')
    elif (type == i_rep):
        stim_code = Move3_none
        temp = counter.get() + incr
        if (temp > limit):
            counter.set(limit)
        else:
            counter.set(temp)
        rep_nr = counter.get()
        #print('rep_nr more')
    TCP_update()


def decrease_stat(event=None, counter=None, incr=None, limit=None, type=None):
    global stim_code, rep_nr, user_code
    user_code = User_none

    if (type == i_ramp):
        stim_code = Move3_ramp_less
        #print('Move3_ramp_less')
    elif (type == i_cur):
        stim_code = Move3_decr
        #print('Move3_decr')
    elif (type == i_rep):
        stim_code = Move3_none
        temp = counter.get() - incr
        if (temp < limit):
            counter.set(limit)
        else:
            counter.set(temp)
        rep_nr = counter.get()
        #print('rep_nr less')
    TCP_update()

def user_button(type=None):
    global stim_code, rep_nr, user_code, User_CM, User_st, Move3_done
    user_code = User_none
    stim_code = Move3_none

    if (type >= User_CM) and (type <= User_st):
        user_code = type
    elif (type >= 10) and (type <= (10+Move3_done)):
        stim_code = type - 10
    else:
        print('Invalid button pressed?')
    TCP_update()

counters = [tk.IntVar() for _ in range(30)]

# Defining buttons and interface
fSize = 20
bSize_w = 3
bSize_h = 1


rnum = 0
tk.Label(root, font=("Arial Bold", 30), text="CH.1", fg=standard_blue).grid(row=rnum, column=1)

rnum += 2
# Ramp button
val_text = "Ramp"
counters[i_ramp].set(init_ramp)
tk.Label(root, font=("Arial Bold", fSize), text=val_text).grid(row=rnum, column=1)
tk.Label(root, font=("Arial Bold", fSize), textvariable=counters[i_ramp]).grid(row=rnum+1, column=1)
# +- buttons # width=20, height=3,
tk.Button(root, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="+", bg=standard_blue, fg="white",
    command=lambda counter=counters[i_ramp]: increase_stat(counter=counter, incr=Dramp, limit=max_ramp, type=i_ramp)).grid(row=rnum+1, column=0)
tk.Button(root, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="-", bg=standard_blue, fg="white",
    command=lambda counter=counters[i_ramp]: decrease_stat(counter=counter, incr=Dramp, limit=min_ramp, type=i_ramp)).grid(row=rnum+1, column=2)

rnum += 2
# Current button
val_text = "Current"
counters[i_cur].set(init_cur)
tk.Label(root, font=("Arial Bold", fSize), text=val_text).grid(row=rnum, column=1)
tk.Label(root, font=("Arial Bold", fSize), textvariable=counters[i_cur]).grid(row=rnum+1, column=1)
# buttons
tk.Button(root, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h,  text="+", bg=standard_blue, fg="white",
    command=lambda counter=counters[i_cur]: increase_stat(counter=counter, incr=Dcur, limit=max_cur, type=i_cur)).grid(row=rnum+1, column=0)
tk.Button(root,font=("Arial Bold", fSize), width=bSize_w, height=bSize_h,  text="-", bg=standard_blue, fg="white",
    command=lambda counter=counters[i_cur]: decrease_stat(counter=counter, incr=Dcur, limit=min_cur, type=i_cur)).grid(row=rnum+1, column=2)

rnum += 2
# Rep button
val_text = "Rep"
counters[i_rep].set(init_rep)
tk.Label(root, font=("Arial Bold", fSize), text=val_text).grid(row=rnum, column=1)
tk.Label(root, font=("Arial Bold", fSize), textvariable=counters[i_rep]).grid(row=rnum+1, column=1)
# buttons
tk.Button(root, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="+", bg=standard_blue, fg="white",
    command=lambda counter=counters[i_rep]: increase_stat(counter=counter, incr=Drep, limit=max_rep, type=i_rep)).grid(row=rnum+1, column=0)
tk.Button(root, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="-", bg=standard_blue, fg="white",
    command=lambda counter=counters[i_rep]: decrease_stat(counter=counter, incr=Drep, limit=min_rep, type=i_rep)).grid(row=rnum+1, column=2)

rnum += 2
# Start, stop button
fSize_ST = 10
bSize_w_ST = 10
bSize_h_ST = 3

tk.Button(root, font=("Arial", fSize_ST), width=bSize_w_ST, height=bSize_h_ST, text="Stim\nStart", bg=standard_green, fg="white",
    command=lambda: user_button(type=(Move3_start+10)) ).grid(row=rnum+1, column=5)
tk.Button(root, font=("Arial", fSize_ST), width=bSize_w_ST, height=bSize_h_ST, text="Stim\nQuit", bg=dark_red, fg="white",
    command=lambda: user_button(type=(Move3_stop+10)) ).grid(row=rnum+1, column=6)


rnum += 2
# Calibration and process buttons
fSize_cal = 10
bSize_w_cal = 10
bSize_h_cal = 3

tk.Button(root, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Manual\nCalibration", fg=lsr_text,
    command=lambda: user_button(type=User_CM)).grid(row=rnum+1, column=5)
tk.Button(root, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Automatic\nCalibration", fg=lsr_text,
    command=lambda: user_button(type=User_CA)).grid(row=rnum+1, column=6)
tk.Button(root, font=("Arial", fSize_cal), width=3, height=bSize_h_cal, text="X", bg=aau_blue, fg="white",
    command=lambda: user_button(type=Move3_done+10)).grid(row=rnum+1, column=7)

tk.Button(root, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Start\nTraining", bg=standard_green, fg="white",
    command=lambda: user_button(type=User_st)).grid(row=rnum+1, column=10)
tk.Button(root, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Set\nThreshold", bg=standard_blue, fg="white",
    command=lambda: user_button(type=User_th)).grid(row=rnum+1, column=11)

rnum += 2
# Image
canvas = Canvas(root, width = 300, height = 300)
img = ImageTk.PhotoImage(Image.open("lsr_logo.jpg"))
#tk.Label(root, image=img).grid(row=rnum, column=5)
root.mainloop()
# Tell the server the program is going to finish
s.sendall(b'ENDTCP')
