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
from GUI_header_v5 import *

global s, TIMEOUT_SEC, UPDATE_PERIOD, start_train
# ========================== Set TCP ==========================
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP socket
s.connect((HOST, PORT)) 

# ========================== Functions ==========================
def update_display():
    # udpate colours and texts on the screen
    global status, statusMessage, active, ch_active, start_train, exercise
    status_value['text'] = '   ' + status_list[status]
    status_display['text'] = 'Info: ' + statusMessage
    # Stimulation parameters
    ramp_value['text'] = str(int(counters[i_ramp].get())) + ' pts'
    cur_value['text'] = str(counters[i_cur].get()) + ' mA'
    fq_value['text'] = str(counters[i_fq].get()) + ' Hz'
    rep_value['text'] = str(int(counters[i_rep].get())) + ' rep'
    # Start/stop stimulator options change colours
    if(active == 1):
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
    else:
        manButton.configure(state="disabled")

    if(status == st_init or status == st_calM or status == st_calA_go or status == st_calA_stop):
        xButton['text'] = "End\nset-up"
    else:
        xButton['text'] = "End\nexercise"
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
        #ex_current['text'] =  'Incoming exercise: ' + ex_select[exercise]
        mth_current['text'] =  'Incoming method: ' + mth_select[method]
    else:
        #ex_current['text'] =  'Current exercise: ' + ex_select[exercise]
        mth_current['text'] =  'Current method: ' + mth_select[method]

    # Select boxes and options while manual calibration is active
    if(status == st_calM):
        startButton.configure(state="normal")
    else:
        startButton.configure(state="disabled")
    

def update_sas():
    global stim_code, rob_status, rep_nr, user_code, ch_str, exercise
    global i_ramp, i_cur, i_fq, counters, active, ch_active
    global start_train, exercise, method
    global i_cnt, status, statusMessage
    # Encode message
    str_rep = ' '
    if rep_nr >= 10:
        str_rep = str(rep_nr)
    else:
        str_rep = '0' + str(rep_nr)

    message = 'SCREEN;' + str(stim_code) + ';' + \
        str(user_code) + ';' + rob_status + ';' + str_rep + ';' + \
        str(ch_select.index(ch_str.get())) + ';' + \
        str(ex_select.index(ex_str.get())) + ';' + \
        str(mth_select.index(mth_str.get())) + '; --;'   
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
            
            # stimulation parameters
            counters[i_cur].set(float(data_ls[1]))
            counters[i_ramp].set(float(data_ls[2]))
            counters[i_fq].set(float(data_ls[3]))
            ch_active = int(data_ls[4])
            active = int(data_ls[5])
            # setting parameters
            start_train = int(data_ls[6])
            exercise = int(data_ls[7])
            method = int(data_ls[8])
            
            # status on the current exercise
            counters[i_cnt].set(float(data_ls[9]))
            status = int(data_ls[10])
            statusMessage = str(data_ls[11])
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
    elif (type == i_cur):
        stim_code = Move3_incr
    elif (type == i_fq):
        stim_code = Move3_Hz_mr
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
    elif (type == i_fq):
        stim_code = Move3_Hz_ls
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
    global Move3_incr, MOVE3_MAX  # max and min stim command values

    user_code = User_none
    stim_code = Move3_none

    if (type >= User_CM) and (type <= User_new):
        user_code = type
    elif (type >= 10) and (type <= (10+MOVE3_MAX)):
        stim_code = type - 10
    else:
        print('Invalid button pressed?')
# ========================== Top frame definition & location ==========================
# Decoration
bottom_label.grid(row=0, columnspan=3) # Copyright

param_title_label['text'] = '  Program status: '
param_title_label.place(relx = 0.0)
status_value.place(relx = 0.0, rely = 0.25)
status_display.place(relx = 0.5)

# ========================== Central frame definition ==========================
trainButton['command'] = lambda: user_button(type=User_st)
thButton['command'] = lambda: user_button(type=User_th)

exButton_repeat['text'] = 'Repeat same\nexercise'
exButton_repeat['command'] = lambda: user_button(type=(User_rep))

exButton_new['text'] = 'Do different\nexercise'
exButton_new['command'] = lambda: user_button(type=(User_new))

# ========================== Central frame location ==========================
marginX = 0.05
next_margin = 0.4
marginY = 0.05
next_marginY = 0.1

frame_title.place(relx = marginX + 0.1, rely = 0)

# Set threshold / start training
marginY += 0.1
trainButton.place(relx = marginX, rely = marginY)
thButton.place(relx = marginX+next_margin, rely = marginY)

# New / repeat exercise
marginY += 0.2
exButton_repeat.place(relx = marginX, rely = marginY)
exButton_new.place(relx = marginX+next_margin, rely = marginY)

# Select threshold method
marginY += 0.2
mth_current.place(relx = marginX, rely = marginY)
mth_next.place(relx = marginX, rely = marginY+next_marginY)
mth_box.place(relx = marginX+next_margin, rely = marginY+next_marginY)

# Select exercise
#marginY += 0.2
#ex_current.place(relx = marginX, rely = marginY)
#ex_next.place(relx = marginX, rely = marginY+next_marginY)
#ex_box.place(relx = marginX+next_margin, rely = marginY+next_marginY)
# =================== Side buttons definition ===================
select_frame = ctr_left

stim_label['text'] = "Stimulation\nparameters"

# Ramp options
counters[i_ramp].set(init_ramp)
ramp_label['text'] = "Ramp"
rampButton_plus['command'] = lambda counter=counters[i_ramp]: increase_stat(counter=counter, incr=Dramp, limit=MAX_RAMP, type=i_ramp)
rampButton_minus['command'] = lambda counter=counters[i_ramp]: decrease_stat(counter=counter, incr=Dramp, limit=MIN_RAMP, type=i_ramp)

# Current options
counters[i_cur].set(init_cur)
curr_label['text'] = "Amplitude"
curButton_plus['command'] = lambda counter=counters[i_cur]: increase_stat(
    counter=counter, incr=Dcur, limit=MAX_CUR, type=i_cur)
curButton_minus['command'] = lambda counter=counters[i_cur]: decrease_stat(
    counter=counter, incr=Dcur, limit=MIN_CUR, type=i_cur)

# Frequency options
counters[i_fq].set(init_fq)
fq_label['text'] = "Frequency"
fqButton_plus['command'] = lambda counter=counters[i_fq]: increase_stat(
    counter=counter, incr=Dcur, limit=MAX_FQ, type=i_fq)
fqButton_minus['command'] = lambda counter=counters[i_fq]: decrease_stat(
    counter=counter, incr=Dfq, limit=MIN_FQ, type=i_fq)

# Repetition options
counters[i_rep].set(init_rep)
rep_label['text'] = "Repetitions"
repButton_plus ['command']= lambda counter = counters[i_rep]: increase_stat(counter=counter, incr=Drep, limit=MAX_REP, type=i_rep)
repButton_minus ['command']= lambda counter=counters[i_rep]: decrease_stat(counter=counter, incr=Drep, limit=MIN_REP, type=i_rep)

# =================== Side buttons location ===================
rnum = 0

stim_label.grid(row=rnum, column = 1)

rnum += 2
# Ramp options
ramp_label.grid(row=rnum, column=1)
ramp_value.grid(row=rnum+1, column=1)
rampButton_plus.grid(row=3, column=2)
rampButton_minus.grid(row=3, column=0)

rnum += 2
# Current options
curr_label.grid(row=rnum, column=1)
curButton_minus.grid(row=rnum+1, column=0)
cur_value.grid(row=rnum+1, column=1)
curButton_plus.grid(row=rnum+1, column=2)

rnum += 2
# Frequency options
fq_label.grid(row=rnum, column=1)
fq_value.grid(row=rnum+1, column=1)
fqButton_plus.grid(row=rnum+1, column=2)
fqButton_minus.grid(row=rnum+1, column=0)

rnum += 2
# Repetition options
rep_label.grid(row=rnum, column=1)
rep_value.grid(row=rnum+1, column=1)
repButton_plus.grid(row=rnum+1, column=2)
repButton_minus.grid(row=rnum+1, column=0)

# =================== Bottom buttons definition ===================
select_frame = btm_frame

# Start, stop button
startButton['command'] = lambda: user_button(type=(Move3_start+10))
quitButton['command'] = lambda: user_button(type=(Move3_stop+10))
# Calibration and process buttons
manButton['text'] = "Set\nstimulation" # "Manual\nCalibration"
manButton['command']  = lambda: user_button(type=User_CM)
xButton['command'] = lambda: user_button(type=Move3_done+10)

# =================== Bottom buttons location ===================
rnum = 0
dummy_label1.grid(row=rnum, column=0)

# Start, stop button
startButton.grid(row=rnum, column=5)
quitButton.grid(row=rnum, column=6)

# Calibration and process buttons
dummy_label2.grid(row=rnum, column=7)

manButton.grid(row=rnum, column=16)
xButton.grid(row=rnum, column=17)

# =====================================================================================
# Schedule TCP update every period
root.after(UPDATE_PERIOD, update_sas)
root.mainloop()
# Tell the server the program is going to finish
s.sendall(b'ENDTCP;--;')
