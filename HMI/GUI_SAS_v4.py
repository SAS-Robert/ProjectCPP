# Standard Python libraries
from tkinter import *
import tkinter as tk
import socket
import time
import math
import decimal
import select
from PIL import ImageTk, Image

# User-defined libraries
from GUI_header import *

# ========================== Set TCP ==========================
global s, TIMEOUT_SEC, UPDATE_PERIOD
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP socket
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP socket
s.connect((HOST, PORT)) 

# ========================== Functions ==========================
#def update_display():
    # udpate stuff here

def update_sas():
    global stim_code, rob_status, rep_nr, user_code, ch_str
    # Encode message
    str_rep = ' '
    if rep_nr >= 10:
        str_rep = str(rep_nr)
    else:
        str_rep = '0' + str(rep_nr)

    message = 'SCREEN;' + str(stim_code) + ';' + \
        str(user_code) + ';' + rob_status + str_rep + ';' + \
        str(ch_select.index(ch_str.get())) + ';'
    #print(f'Sending... {message}')
    s.sendall(message.encode("utf-8"))
    # Decode message
    #print('Receiving...')
    global TIMEOUT_SEC
    ready = select.select([s], [], [], TIMEOUT_SEC)
    if ready[0]:
        data = s.recv(512)
        #print('Received', repr(data.decode("utf-8")))
        #data_message = data.decode("utf-8")
        data_ls = (data.decode("utf-8")).split(";")
        #print(data_ls)
        if(data_ls[0] == 'SAS'):
            #print('Beginning is goood')
            temp_cur = float(data_ls[1])
            temp_ramp = float(data_ls[2])
            global i_ramp, i_cur, counters
            counters[i_ramp].set(temp_ramp)
            counters[i_cur].set(temp_cur)
    else:
        print('Timeout')
    # Update variables
    stim_code = Move3_none
    user_code = User_none
    # New
    root.after(UPDATE_PERIOD, update_sas)


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


def decrease_stat(event=None, counter=None, incr=None, limit=None, type=None):
    global stim_code, rep_nr, user_code
    user_code = User_none

    if (type == i_ramp):
        stim_code = Move3_ramp_less
    elif (type == i_cur):
        stim_code = Move3_decr
    elif (type == i_rep):
        stim_code = Move3_none
        temp = counter.get() - incr
        if (temp < limit):
            counter.set(limit)
        else:
            counter.set(temp)
        rep_nr = counter.get()


def user_button(type=None):
    global stim_code, rep_nr, user_code, User_CM, User_st, Move3_en_ch
    user_code = User_none
    stim_code = Move3_none

    if (type >= User_CM) and (type <= User_st):
        user_code = type
    elif (type >= 10) and (type <= (10+Move3_en_ch)):
        stim_code = type - 10
    else:
        print('Invalid button pressed?')
# ========================== Objects location ==========================
# Decoration
banner_label.grid(row=0, column=0)
param_title_label.grid(row=0, columnspan=3)
param_label.grid(row=1, column=1) # Image file
bottom_label.grid(row=0, columnspan=3)


# =================== Side buttons definition ===================
select_frame = ctr_left
# Ramp options
val_text = "Ramp"
counters[i_ramp].set(init_ramp)
ramp_label = tk.Label(select_frame, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg,
                      text=val_text)
ramp_value = tk.Label(select_frame, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg,
                      textvariable=counters[i_ramp])
# +- buttons # width=20, height=3,
rampButton_plus = tk.Button(select_frame, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="+", bg=STANDARD_BLUE, fg=ctrl_left_fg,
                            command=lambda counter=counters[i_ramp]: increase_stat(counter=counter, incr=Dramp, limit=MAX_RAMP, type=i_ramp))

rampButton_minus = tk.Button(select_frame, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="-", bg=STANDARD_BLUE, fg=ctrl_left_fg,
                             command=lambda counter=counters[i_ramp]: decrease_stat(counter=counter, incr=Dramp, limit=MIN_RAMP, type=i_ramp))

# Current options
val_text = "Current"
counters[i_cur].set(init_cur)
curr_label = tk.Label(select_frame, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg,
         text=val_text)
         
cur_value = tk.Label(select_frame, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg,
         textvariable=counters[i_cur])
curButton_plus = tk.Button(select_frame, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h,  text="+", bg=STANDARD_BLUE, fg=ctrl_left_fg,
          command=lambda counter=counters[i_cur]: increase_stat(counter=counter, incr=Dcur, limit=MAX_CUR, type=i_cur))
curButton_minus = tk.Button(select_frame, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h,  text="-", bg=STANDARD_BLUE, fg=ctrl_left_fg,
          command=lambda counter=counters[i_cur]: decrease_stat(counter=counter, incr=Dcur, limit=MIN_CUR, type=i_cur))

# Repetition options
val_text = "Rep"
counters[i_rep].set(init_rep)
rep_label = tk.Label(ctr_left, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg,
                     text=val_text)
rep_value = tk.Label(ctr_left, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg,
                     textvariable=counters[i_rep])
repButton_plus = tk.Button(ctr_left, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="+", bg=STANDARD_BLUE, fg=ctrl_left_fg,
                           command=lambda counter=counters[i_rep]: increase_stat(counter=counter, incr=Drep, limit=MAX_REP, type=i_rep))
repButton_minus = tk.Button(ctr_left, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="-", bg=STANDARD_BLUE, fg=ctrl_left_fg,
                            command=lambda counter=counters[i_rep]: decrease_stat(counter=counter, incr=Drep, limit=MIN_REP, type=i_rep))

# (dis) Enable channel
enButton = tk.Button(ctr_left, font=("Arial Bold", 13), width=10, height=2, text="(dis) enable\nChannel", bg=AAU_BLUE, fg=ctrl_left_fg,
                     command=lambda: user_button(type=(Move3_en_ch+10)))

# =================== Bottom buttons definition ===================
select_frame = btm_frame
# Start, stop button
fSize_ST = 13
bSize_w_ST = 10
bSize_h_ST = 3

startButton = tk.Button(select_frame, font=("Arial", fSize_ST), width=bSize_w_ST, height=bSize_h_ST, text="Stim\nStart", bg=LSR_PLUS, fg="white",
          command=lambda: user_button(type=(Move3_start+10)))
quitButton = tk.Button(select_frame, font=("Arial", fSize_ST), width=bSize_w_ST, height=bSize_h_ST, text="Stim\nQuit", bg=DARK_RED, fg="white",
                       command=lambda: user_button(type=(Move3_stop+10)))
# Calibration and process buttons
dummy_color = LSR_TEXT
fSize_cal = 13
bSize_w_cal = 10
bSize_h_cal = 3

dummy_label1 = Label(select_frame, text='        ',
                     font=("Arial Bold", 16), bg=dummy_color)

manButton = tk.Button(select_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Manual\nCalibration", fg=LSR_TEXT, bg=ctrl_left_fg,
                      command=lambda: user_button(type=User_CM))
autoButton = tk.Button(select_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Automatic\nCalibration", fg=LSR_TEXT, bg=ctrl_left_fg,
                       command=lambda: user_button(type=User_CA))
xButton = tk.Button(select_frame, font=("Arial", fSize_cal), width=bSize_w_cal-3, height=bSize_h_cal, text="X", bg=AAU_BLUE, fg="white",
                    command=lambda: user_button(type=Move3_done+10))

dummy_label2 = Label(select_frame, text='        ', font=(
    "Arial Bold", 16), bg=dummy_color)

trainButton = tk.Button(select_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Start\nTraining", bg=LSR_PLUS, fg="white",
                        command=lambda: user_button(type=User_st))
thButton = tk.Button(select_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Set\nThreshold", bg=LSR_LINE, fg="white",
                     command=lambda: user_button(type=User_th))

# =================== Side buttons location ===================
rnum = 0

# Select stimulator channel
ch_box.grid(row=rnum, column=1)

rnum += 2
# Ramp options
ramp_label.grid(row=rnum, column=1)
ramp_value.grid(row=rnum+1, column=1)
rampButton_plus.grid(row=rnum+1, column=2)
rampButton_minus.grid(row=rnum+1, column=0)

rnum += 2
# Current options
curr_label.grid(row=rnum, column=1)
curButton_minus.grid(row=rnum+1, column=0)
cur_value.grid(row=rnum+1, column=1)
curButton_plus.grid(row=rnum+1, column=2)

rnum += 2
# Repetition options
rep_label.grid(row=rnum, column=1)
rep_value.grid(row=rnum+1, column=1)
repButton_plus.grid(row=rnum+1, column=2)
repButton_minus.grid(row=rnum+1, column=0)

rnum += 2
# (Dis) Enable button
enButton.grid(row=rnum+1, column=1)

# =================== Bottom buttons location ===================
rnum = 0
# Start, stop button
startButton.grid(row=rnum, column=5)
quitButton.grid(row=rnum, column=6)


# Calibration and process buttons
dummy_label1.grid(row=rnum, column=7)

manButton.grid(row=rnum, column=15)
autoButton.grid(row=rnum, column=16)
xButton.grid(row=rnum, column=17)

dummy_label2.grid(row=rnum, column=18)

trainButton.grid(row=rnum, column=20)
thButton.grid(row=rnum, column=21)


# =====================================================================================
# Schedule TCP update every period
root.after(UPDATE_PERIOD, update_sas)
root.mainloop()
# Tell the server the program is going to finish
s.sendall(b'ENDTCP')
