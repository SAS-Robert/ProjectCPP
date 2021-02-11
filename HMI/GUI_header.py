# Standard Python libraries
from tkinter import *
import tkinter as tk
import time
import math
import decimal
import select
from PIL import ImageTk, Image

# ========================== Set TCP ==========================
HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 30002        # The port used by the server
global TIMEOUT_SEC, UPDATE_PERIOD
TIMEOUT_SEC = 0.2
UPDATE_PERIOD = 210

# ========================== TCP variables ==========================
# state_Type
global st_init, st_th, st_wait, st_running, st_stop, st_end, st_calM, st_calA_go, st_calA_stop, st_repeat
st_init = 0      
st_th = 1
st_wait = 2    
st_running = 3      
st_stop = 4
st_end = 5
st_calM = 6
st_calA_go = 7
st_calA_stop = 8
st_repeat = 9

# exercise_Type
global exUnregistered, lowerLeg_flex, upperLeg_ext
exUnregistered = 0
lowerLeg_flex = 1 
upperLeg_ext = 2

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
# User_Req_Type
global User_none, User_CM, User_CA, User_X, User_th, User_st
User_none = 0
User_CM = 1
User_CA = 2
User_X = 3
User_th = 4
User_st = 5

# Initialize
global stim_code, rob_status, rep_nr, user_code, exercise, status, statusMessage
rob_status = 'R'
rep_nr = 20
stim_code = Move3_none
user_code = User_none
exercise = exUnregistered
status = st_init
statusMessage = " "

global counters, active

global CH_RED, CH_BLUE, CH_BLACK, CH_WHITE
CH_RED = 0
CH_BLUE = 1
CH_BLACK = 2
CH_WHITE = 3
# ========================== Internal variables ==========================
global MAX_REP, MIN_REP, MAX_CUR, MIN_CUR, MAX_RAMP, MIN_RAMP
global Drep, Dcur, Dramp, i_rep, i_cur, i_ramp, init_cur, init_ramp, init_rep
MAX_REP = 99
MIN_REP = 1
MAX_CUR = 5.0
MIN_CUR = 0.0
MAX_RAMP = 3
MIN_RAMP = 1
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
i_status = 4

# Display lists
global ch_select, ch_str, ex_select, status_list
ch_select = ["Ch.Red", "Ch.Blue", "Ch.Black", "Ch.White"]
ex_select = ["Other", "Lower leg flexion", "Upper leg extension"]
status_list = ["Initialization", "Setting threshold", "EMG monitoring", "Tigger FES", "Stop FES", "Finish program", "Manual calibration active", "Automatic calibration - stimulating", "Automatic calibration - resting", "Exercise finished"]

# Colours
LIGHT_RED = '#D95319'
DARK_RED = '#A2142F'
STANDARD_GREEN = '#77AC30'
STANDARD_BLUE = '#0052cc'
GOLDY = '#EDB120'
PURPLY = '#7E2F8E'
LSR_TEXT = '#15568a'
LSR_LINE = '#25a9d1'
LSR_PLUS = '#0da140'
AAU_BLUE = '#211a52'
MEDIUM_GREY = '#808080'
DARK_GREY = '#404040'

# ========================== Functions ==========================
#
# ========================== Window layout ==========================
root = Tk()
root.title('GUI SAS')
root.geometry('{}x{}'.format(1000, 650))

# create all of the main containers
top_frame = Frame(root, bg='black', width=1000, height=100, pady=1)
center = Frame(root, bg='gray2', width=50, height=40, padx=3, pady=1)
btm_frame = Frame(root, bg=LSR_TEXT, width=450, height=45, pady=1)
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

ctrl_left_color = LSR_TEXT
ctrl_left_fg = 'white'
ctr_mid_color = 'white'
ctr_mid_fg = 'black'
ctr_right_color = 'green'

ctr_left = Frame(center, bg=ctrl_left_color, width=100, height=190)
ctr_mid = Frame(center, bg=ctr_mid_color, width=250,
                height=190, padx=3, pady=3)
#ctr_right = Frame(center, bg=ctr_right_color, width=100, height=190, padx=3, pady=3)

ctr_left.grid(row=0, column=0, sticky="ns")
ctr_mid.grid(row=0, column=1, sticky="nsew")
#ctr_right.grid(row=0, column=2, sticky="ns")

# =================== Window decoration ===================
# the top frame
canvas = Canvas(root, width=1000, height=100)
img_banner = ImageTk.PhotoImage(Image.open("banner.jpg"))
banner_label = Label(top_frame, image=img_banner, bg='black')

# center middle
param_title_label = Label(ctr_mid, font=("Arial Italic", 34), bg=ctr_mid_color, fg=AAU_BLUE)

# bottom end
c_symbol = "\u00a9"
text_copy = 'Copyright:' + c_symbol + \
    "2020 Life Science Robotics " + '&' + " Aalborg University."
bottom_label = Label(btm_frame2, text=text_copy, bg='lavender', fg=AAU_BLUE)

# Buttons on the blue side
counters = [tk.IntVar() for _ in range(30)]

# =================== Center description ===================
select_frame = ctr_mid
fSize = 15
bSize_w = 3
bSize_h = 1

status_value = tk.Label(select_frame, font=(
    "Arial Bold", fSize), bg=ctr_mid_color, fg=ctr_mid_fg)
status_display = tk.Label(select_frame, font=(
    "Arial Bold", 10), bg=ctr_mid_color, fg=ctr_mid_fg)

exButton_repeat = tk.Button(select_frame, font=("Arial Bold", fSize), width=10,
                           height=2, bg=STANDARD_BLUE, fg=ctrl_left_fg)

exButton_new = tk.Button(select_frame, font=("Arial Bold", fSize), width=10,
                           height=2, bg=STANDARD_BLUE, fg=ctrl_left_fg)

# =================== Side buttons description ===================
select_frame = ctr_left
# Defining buttons size and their font
fSize = 20
bSize_w = 3
bSize_h = 1

# Select stimulator channel
ch_str = StringVar(root)
ch_str.set(ch_select[CH_RED])
ch_box = OptionMenu(select_frame, ch_str, *ch_select)
ch_box.configure(width=6, font=("Arial Bold", 20),
                 bg=ctrl_left_color, fg=ctrl_left_fg)

# Ramp options
counters[i_ramp].set(init_ramp)
ramp_label = tk.Label(select_frame, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg)
ramp_value = tk.Label(select_frame, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg)
# +- buttons # width=20, height=3,
rampButton_plus = tk.Button(select_frame, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="+", bg=STANDARD_BLUE, fg=ctrl_left_fg)
rampButton_minus = tk.Button(select_frame, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="-", bg=STANDARD_BLUE, fg=ctrl_left_fg)

# Current options
curr_label = tk.Label(select_frame, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg)

cur_value = tk.Label(select_frame, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg,
                     textvariable=counters[i_cur])
curButton_plus = tk.Button(select_frame, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h,  text="+", bg=STANDARD_BLUE, fg=ctrl_left_fg)
curButton_minus = tk.Button(select_frame, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h,  text="-", bg=STANDARD_BLUE, fg=ctrl_left_fg)

# Repetitions options
rep_label = tk.Label(select_frame, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg)
rep_value = tk.Label(select_frame, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg)
repButton_plus = tk.Button(select_frame, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="+", bg=STANDARD_BLUE, fg=ctrl_left_fg)
repButton_minus = tk.Button(select_frame, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h, text="-", bg=STANDARD_BLUE, fg=ctrl_left_fg)

enButton = tk.Button(select_frame, font=("Arial Bold", 13), width=10,
                     height=2, text="(dis) enable\nChannel", bg=AAU_BLUE, fg=ctrl_left_fg)

# =================== Bottom buttons description ===================
select_frame = btm_frame

# Start, stop button
fSize_ST = 13
bSize_w_ST = 10
bSize_h_ST = 3

startButton = tk.Button(select_frame, font=("Arial", fSize_ST), width=bSize_w_ST, height=bSize_h_ST, text="Stim\nStart", bg=LSR_PLUS, fg="white")
quitButton = tk.Button(select_frame, font=("Arial", fSize_ST), width=bSize_w_ST, height=bSize_h_ST, text="Stim\nQuit", bg=DARK_RED, fg="white")

# Calibration and process buttons
dummy_color = LSR_TEXT
fSize_cal = 13
bSize_w_cal = 10
bSize_h_cal = 3

dummy_label1 = Label(select_frame, text='        ',
                     font=("Arial Bold", 16), bg=dummy_color)

manButton = tk.Button(select_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Manual\nCalibration", fg=LSR_TEXT, bg=ctrl_left_fg)
autoButton = tk.Button(select_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Automatic\nCalibration", fg=LSR_TEXT, bg=ctrl_left_fg)
xButton = tk.Button(select_frame, font=("Arial", fSize_cal), width=bSize_w_cal-3, height=bSize_h_cal, text="X", bg=AAU_BLUE, fg="white")

dummy_label2 = Label(select_frame, text='        ', font=(
    "Arial Bold", 16), bg=dummy_color)

trainButton = tk.Button(select_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Start\nTraining", bg=LSR_PLUS, fg="white")
thButton = tk.Button(select_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Set\nThreshold", bg=LSR_LINE, fg="white")
