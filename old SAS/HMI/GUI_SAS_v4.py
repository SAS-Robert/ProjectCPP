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

global s, TIMEOUT_SEC, UPDATE_PERIOD, start_train
# ========================== Set TCP ==========================
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP socket
#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP socket
s.connect((HOST, PORT)) 

# ========================== Functions ==========================
def update_display():
    # udpate colours and texts on the screen
    global status, statusMessage, active, ch_active, start_train, exercise
    status_value['text'] = status_list[status]
    status_display['text'] = 'Info: ' + statusMessage

    if (ch_active == 1.0):
        enButton['text'] = "Disable\nchannel"
    else:
        enButton['text'] = "Enable\nchannel"
    if(active == 0 and ch_active == 0):
        enButton['bg'] = LSR_PLUS
    else:
        enButton['bg'] = AAU_BLUE

    # Start/stop stimulator options change colours
    if(active == 1.0):
        quitButton['bg'] = DARK_RED
    else:
        quitButton['bg'] = MEDIUM_GREY

    if(active == 0 and ch_active == 1 and status == st_calM):
        startButton['bg'] = LSR_PLUS
    else:
        startButton['bg'] = MEDIUM_GREY

    # Manual / Automatic calibration
    if (status == st_init):
        manButton.configure(state="normal")
        autoButton.configure(state="normal")
    else:
        manButton.configure(state="disabled")
        autoButton.configure(state="disabled")

    if(status == st_init or status == st_calM or status == st_calA_go or status == st_calA_stop):
        xButton['text'] = "Quit\ncalibration"
    else:
        xButton['text'] = "Quit\nexercise"
    if(status == st_init or status == st_repeat or status == st_th):
        xButton.configure(state="disabled")
    else:
        xButton.configure(state="normal")

    # Other buttons
    if(status == st_calM or status == st_repeat):
        thButton.configure(state="normal")
    else:
        thButton.configure(state="disabled")

    if (status == st_wait and start_train == 0):
        trainButton.configure(state="normal")
    else:
        trainButton.configure(state="disabled")

    if(status == st_repeat):
        exButton_repeat.configure(state="normal")
        exButton_new.configure(state="normal")
    else:
        exButton_repeat.configure(state="disabled")
        exButton_new.configure(state="disabled")

    if(status == st_init or status == st_repeat):
        ex_current['text'] =  'Incoming exercise: ' + str(ex_select[exercise])
    else:
        ex_current['text'] =  'Current exercise: ' + str(ex_select[exercise])

    # Select boxes and options while manual calibration is active
    if(status == st_calM):
        ch_box.configure(state="normal")
        enButton.configure(state="normal")
        startButton.configure(state="normal")
    else:
        ch_str.set(ch_select[CH_RED])
        ch_box.configure(state="disabled")
        enButton.configure(state="disabled")
        startButton.configure(state="disabled")
    


def update_sas():
    global stim_code, rob_status, rep_nr, user_code, ch_str, exercise
    # Encode message
    str_rep = ' '
    if rep_nr >= 10:
        str_rep = str(rep_nr)
    else:
        str_rep = '0' + str(rep_nr)

    message = 'SCREEN;' + str(stim_code) + ';' + \
        str(user_code) + ';' + rob_status + str_rep + ';' + \
        str(ch_select.index(ch_str.get())) + ';' + \
        str(ex_select.index(ex_str.get()))
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
            global i_ramp, i_cur, counters, active, ch_active, exercise, status, statusMessage, start_train
            #print('Beginning is goood')
            temp_cur = float(data_ls[1])
            temp_ramp = float(data_ls[2])
            # stimulator parameters 
            counters[i_ramp].set(temp_ramp)
            counters[i_cur].set(temp_cur)
            ch_active = int(data_ls[3])
            active = int(data_ls[4])
            # status parameters 
            start_train = int(data_ls[5])
            exercise = int(data_ls[6])
            temp_status = float(data_ls[7])
            status = int(temp_status)
            statusMessage = str(data_ls[8])
    else:
        print('Timeout')
    # Update variables
    stim_code = Move3_none
    user_code = User_none
    update_display()
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
    global stim_code, rep_nr, user_code, status
    global User_CM, User_new        # max and min user command values
    global Move3_incr, Move3_en_ch  # max and min stim command values

    user_code = User_none
    stim_code = Move3_none

    if (type >= User_CM) and (type <= User_new):
        user_code = type
    elif (type >= 10) and (type <= (10+Move3_en_ch)):
        stim_code = type - 10
    else:
        print('Invalid button pressed?')
# ========================== Central frame ==========================
# Decoration
banner_label.grid(row=0, column=0) # Image file
bottom_label.grid(row=0, columnspan=3) # Copyright

param_title_label['text'] = '  Program status: '
param_title_label.place(relx = 0.0)
status_value.place(relx = 0.2)
status_display.place(relx = 0.41)

exButton_repeat['text'] = 'Repeat\nexercise'
exButton_repeat['command'] = lambda: user_button(type=(User_rep))

exButton_new['text'] = 'Do another\nexercise'
exButton_new['command'] = lambda: user_button(type=(User_new))

#exButton_repeat.grid(row=5, column=1)
#exButton_new.grid(row=6, column=1)
exButton_repeat.place(relx = 0.15, rely = 0.1)
exButton_new.place(relx = 0.15, rely = 0.3)

# Select exercise
#ex_current.grid(row=7, column=0)
#ex_next.grid(row=8, column=0)
#ex_box.grid(row=8, column=10)
ex_current.place(relx = 0.15, rely = 0.5)
ex_next.place(relx = 0.15, rely = 0.6)
ex_box.place(relx = 0.15, rely = 0.7)

# =================== Side buttons definition ===================
select_frame = ctr_left

# Ramp options
counters[i_ramp].set(init_ramp)
ramp_label['text'] = "Ramp"
ramp_value['textvariable'] = counters[i_ramp]
rampButton_plus['command'] = lambda counter=counters[i_ramp]: increase_stat(counter=counter, incr=Dramp, limit=MAX_RAMP, type=i_ramp)
rampButton_minus['command'] = lambda counter=counters[i_ramp]: decrease_stat(counter=counter, incr=Dramp, limit=MIN_RAMP, type=i_ramp)


# Current options
counters[i_cur].set(init_cur)
curr_label['text'] = "Current"
cur_value['textvariable'] = counters[i_cur]
curButton_plus['command'] = lambda counter=counters[i_cur]: increase_stat(
    counter=counter, incr=Dcur, limit=MAX_CUR, type=i_cur)
curButton_minus['command'] = lambda counter=counters[i_cur]: decrease_stat(
    counter=counter, incr=Dcur, limit=MIN_CUR, type=i_cur)


# Repetition options
counters[i_rep].set(init_rep)
rep_label['text'] = "Rep"
rep_value['textvariable'] = counters[i_rep]
repButton_plus ['command']= lambda counter = counters[i_rep]: increase_stat(counter=counter, incr=Drep, limit=MAX_REP, type=i_rep)
repButton_minus ['command']= lambda counter=counters[i_rep]: decrease_stat(counter=counter, incr=Drep, limit=MIN_REP, type=i_rep)

# (dis) Enable channel
enButton['command'] =lambda: user_button(type=(Move3_en_ch+10))

# =================== Bottom buttons definition ===================
select_frame = btm_frame

# Start, stop button

startButton['command'] = lambda: user_button(type=(Move3_start+10))
quitButton['command'] = lambda: user_button(type=(Move3_stop+10))
# Calibration and process buttons
manButton['command']  = lambda: user_button(type=User_CM)
autoButton['command'] = lambda: user_button(type=User_CA)
xButton['command'] = lambda: user_button(type=Move3_done+10)

trainButton['command'] = lambda: user_button(type=User_st)
thButton['command'] = lambda: user_button(type=User_th)

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
