from tkinter import *
import tkinter as tk
import socket
import time
import math
import decimal
import select
from PIL import ImageTk, Image

# ========================== Set TCP ==========================
HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 30002        # The port used by the server


global s, timeout_in_seconds, tcp_period
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
timeout_in_seconds = 0.2
tcp_period = 210
# ========================== TCP variables ==========================
# RehaMove3_Req_Type
global Move3_none, Move3_incr, Move3_decr, Move3_ramp_more, Move3_ramp_less, Move3_stop, Move3_start, Move3_done, Move3_en_ch
Move3_none = 0
Move3_incr = 1
Move3_decr = 2
Move3_ramp_more = 3
Move3_ramp_less = 4
Move3_stop = 5
Move3_start = 6
Move3_done = 7
Move3_en_ch = 8 
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

global ch_red, ch_blue, ch_black, ch_white
ch_red = 0
ch_blue = 1
ch_black = 2
ch_white = 3
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

# Stimulator multichannel list
global ch_select, ch_str
ch_select =["Ch.Red", "Ch.Blue", "Ch.Black", "Ch.White"]

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

# ========================== Functions ==========================
def TCP_update():
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
    global timeout_in_seconds
    ready = select.select([s], [], [], timeout_in_seconds)
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
    root.after(tcp_period, TCP_update)


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

# ========================== Set Window layout ==========================
root = Tk()
root.title('GUI SAS')
root.geometry('{}x{}'.format(1000, 650))

# create all of the main containers
top_frame = Frame(root, bg='black', width=1000, height=100, pady=1)
center = Frame(root, bg='gray2', width=50, height=40, padx=3, pady=1)
btm_frame = Frame(root, bg=lsr_text, width=450, height=45, pady=1)
btm_frame2 = Frame(root, bg='lavender', width=1000, height=60, pady=1)

# layout all of the main containers
root.grid_rowconfigure(1, weight=1)
root.grid_columnconfigure(0, weight=1)

top_frame.grid(row=0, sticky="ew")
center.grid(row=1, sticky="nsew")
btm_frame.grid(row=3, sticky="ew")
btm_frame2.grid(row=4, sticky="ew")

# create the center widgets
center.grid_rowconfigure(0, weight=1)
center.grid_columnconfigure(1, weight=1)

ctrl_left_color = lsr_text
ctrl_left_fg = 'white'
ctr_mid_color = 'white'
ctr_mid_fg = 'black'
ctr_right_color = 'green'

ctr_left = Frame(center, bg=ctrl_left_color, width=100, height=190)
ctr_mid = Frame(center, bg=ctr_mid_color, width=250, height=190, padx=3, pady=3)
# ctr_right = Frame(center, bg=ctr_right_color, width=100, height=190, padx=3, pady=3)

ctr_left.grid(row=0, column=0, sticky="ns")
ctr_mid.grid(row=0, column=1, sticky="nsew")
# ctr_right.grid(row=0, column=2, sticky="ns")

# =================== Window decoration ===================
# the top frame
canvas = Canvas(root, width=1000, height=100)
img_banner = ImageTk.PhotoImage(Image.open("banner.jpg"))
banner_label = Label(top_frame, image=img_banner, bg='black')
banner_label.grid(row=0, column=0)

# center middle
param_title_label = Label(ctr_mid, text=' Stimulation parameters',
                          font=("Arial Italic", 34), bg=ctr_mid_color, fg=aau_blue)
param_title_label.grid(row=0, columnspan=3)

canvas = Canvas(root, width=300, height=300)
img_param = ImageTk.PhotoImage(Image.open("HMI_diag1.jpg"))
param_label = Label(ctr_mid, image=img_param)
param_label.grid(row=1, column=1)


# complete botton
# canvas2 = Canvas(root, width=300, height=300)
# logo_im = ImageTk.PhotoImage(Image.open("lsr_logo.jpg"))
# logo_label = Label(btm_frame2, image=logo_im)
# logo_label.grid(row=1, column=1)
c_symbol = "\u00a9"
text_copy = 'Copyright:' + c_symbol + "2020 Life Science Robotics " + '&' + " Aalborg University."
title_label = Label(btm_frame2, text=text_copy, bg='lavender', fg=aau_blue)
title_label.grid(row=0, columnspan=3)

# =================== Side buttons ===================
# Buttons on the blue side
counters = [tk.IntVar() for _ in range(30)]
# Defining buttons size and their font
fSize = 20
bSize_w = 3
bSize_h = 1

rnum = 0
# Select stimulator channel
ch_str = StringVar(root)
ch_str.set(ch_select[ch_red])
ch_box = OptionMenu(ctr_left, ch_str, *ch_select)
ch_box.configure(width=6, font=("Arial Bold", 20), bg=ctrl_left_color,fg=ctrl_left_fg)
ch_box.grid(row=rnum, column=1)

rnum += 2
# Ramp button
val_text = "Ramp"
counters[i_ramp].set(init_ramp)
tk.Label(ctr_left, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg,
         text=val_text).grid(row=rnum, column=1)
tk.Label(ctr_left, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg,
         textvariable=counters[i_ramp]).grid(row=rnum+1, column=1)
# +- buttons # width=20, height=3,
tk.Button(ctr_left, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="+", bg=standard_blue, fg=ctrl_left_fg,
          command=lambda counter=counters[i_ramp]: increase_stat(counter=counter, incr=Dramp, limit=max_ramp, type=i_ramp)).grid(row=rnum+1, column=0)
tk.Button(ctr_left, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="-", bg=standard_blue, fg=ctrl_left_fg,
          command=lambda counter=counters[i_ramp]: decrease_stat(counter=counter, incr=Dramp, limit=min_ramp, type=i_ramp)).grid(row=rnum+1, column=2)

rnum += 2
# Current button
val_text = "Current"
counters[i_cur].set(init_cur)
tk.Label(ctr_left, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg,
         text=val_text).grid(row=rnum, column=1)
tk.Label(ctr_left, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg,
         textvariable=counters[i_cur]).grid(row=rnum+1, column=1)
# buttons
tk.Button(ctr_left, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h,  text="+", bg=standard_blue, fg=ctrl_left_fg,
          command=lambda counter=counters[i_cur]: increase_stat(counter=counter, incr=Dcur, limit=max_cur, type=i_cur)).grid(row=rnum+1, column=0)
tk.Button(ctr_left, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h,  text="-", bg=standard_blue, fg=ctrl_left_fg,
          command=lambda counter=counters[i_cur]: decrease_stat(counter=counter, incr=Dcur, limit=min_cur, type=i_cur)).grid(row=rnum+1, column=2)

rnum += 2
# Rep button
val_text = "Rep"
counters[i_rep].set(init_rep)
tk.Label(ctr_left, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg,
         text=val_text).grid(row=rnum, column=1)
tk.Label(ctr_left, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg,
         textvariable=counters[i_rep]).grid(row=rnum+1, column=1)
# buttons
tk.Button(ctr_left, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="+", bg=standard_blue, fg=ctrl_left_fg,
          command=lambda counter=counters[i_rep]: increase_stat(counter=counter, incr=Drep, limit=max_rep, type=i_rep)).grid(row=rnum+1, column=0)
tk.Button(ctr_left, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="-", bg=standard_blue, fg=ctrl_left_fg,
          command=lambda counter=counters[i_rep]: decrease_stat(counter=counter, incr=Drep, limit=min_rep, type=i_rep)).grid(row=rnum+1, column=2)

rnum += 2
# (Dis) Enable button
tk.Button(ctr_left, font=("Arial Bold", 13), width=10, height=2, text="(dis) enable\nChannel", bg=aau_blue, fg=ctrl_left_fg,
          command=lambda: user_button(type=(Move3_en_ch+10))).grid(row=rnum+1, column=1)

# =================== Bottom buttons ===================
rnum = 0
# Start, stop button
fSize_ST = 13
bSize_w_ST = 10
bSize_h_ST = 3

tk.Button(btm_frame, font=("Arial", fSize_ST), width=bSize_w_ST, height=bSize_h_ST, text="Stim\nStart", bg=lsr_plus, fg="white",
          command=lambda: user_button(type=(Move3_start+10))).grid(row=rnum, column=5)
tk.Button(btm_frame, font=("Arial", fSize_ST), width=bSize_w_ST, height=bSize_h_ST, text="Stim\nQuit", bg=dark_red, fg="white",
          command=lambda: user_button(type=(Move3_stop+10))).grid(row=rnum, column=6)


# Calibration and process buttons
dummy_color = lsr_text
fSize_cal = 13
bSize_w_cal = 10
bSize_h_cal = 3

dummy_label1 = Label(btm_frame, text='        ', font=("Arial Bold", 16), bg=dummy_color).grid(row=rnum, column=7)

tk.Button(btm_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Manual\nCalibration", fg=lsr_text, bg=ctrl_left_fg,
          command=lambda: user_button(type=User_CM)).grid(row=rnum, column=15)
tk.Button(btm_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Automatic\nCalibration", fg=lsr_text, bg=ctrl_left_fg,
          command=lambda: user_button(type=User_CA)).grid(row=rnum, column=16)
tk.Button(btm_frame, font=("Arial", fSize_cal), width=bSize_w_cal-3, height=bSize_h_cal, text="X", bg=aau_blue, fg="white",
          command=lambda: user_button(type=Move3_done+10)).grid(row=rnum, column=17)

dummy_label2 = Label(btm_frame, text='        ', font=("Arial Bold", 16), bg=dummy_color).grid(row=rnum, column=18)

tk.Button(btm_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Start\nTraining", bg=lsr_plus, fg="white",
          command=lambda: user_button(type=User_st)).grid(row=rnum, column=20)
tk.Button(btm_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Set\nThreshold", bg=lsr_line, fg="white",
          command=lambda: user_button(type=User_th)).grid(row=rnum, column=21)


# =====================================================================================
# Schedule TCP update every period
root.after(tcp_period, TCP_update)
root.mainloop()
# Tell the server the program is going to finish
s.sendall(b'ENDTCP')
