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
UPDATE_PERIOD = 150
MAIN_END = False
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

# threshold_Type
global th_SD05, th_SD03, th_other
th_SD05 = 0
th_SD03 = 1
th_other = 2

# RehaMove3_Req_Type
global Move3_none, Move3_incr, Move3_decr, Move3_ramp_more, Move3_ramp_less, Move3_stop, Move3_start, Move3_done, Move3_en_ch
global Move3_Hz_mr, Move3_Hz_ls, Move3_us_mr, Move3_us_ls, MOVE3_MAX
Move3_none = 0
Move3_incr = 1
Move3_decr = 2
Move3_ramp_more = 3
Move3_ramp_less = 4
Move3_stop = 5
Move3_start = 6
Move3_done = 7
Move3_Hz_mr = 8
Move3_Hz_ls = 9
Move3_us_mr = 10
Move3_us_ls = 11
Move3_en_ch = 12

MOVE3_MAX = Move3_Hz_ls # only until freq optios avaliable

# User_Req_Type
global User_none, User_CM, User_CA, User_X, User_th, User_st, User_rep, User_new
User_none = 0
User_CM = 1
User_CA = 2
User_X = 3
User_th = 4
User_st = 5
User_rep = 7
User_new = 8 

# Stimulation parameters and stimulator status
global counters, active

global CH_RED, CH_BLUE, CH_BLACK, CH_WHITE
CH_RED = 0
CH_BLUE = 1
CH_BLACK = 2
CH_WHITE = 3

# exercise_Type
global EX_LOWERLEG, EX_UPPERLEG, EX_CIRCUIT
EX_LOWERLEG = 0
EX_UPPERLEG = 1
EX_CIRCUIT = 3

# Initialize
global stim_code, rob_status, rep_nr, user_code, exercise, method, status, statusMessage
rob_status = 'R'
rep_nr = 10
rep_cnt = 10
stim_code = Move3_none
user_code = User_none
exercise = EX_LOWERLEG
method = th_SD05
status = st_init
statusMessage = " "
# ========================== Internal variables ==========================
global MAX_REP, MIN_REP, MAX_CUR, MIN_CUR, MAX_RAMP, MIN_RAMP
global Drep, Dcur, Dramp, i_rep, i_cur, i_ramp, init_cur, init_ramp, init_rep

# Increment and initial values
MAX_REP = 99
MIN_REP = 1
Drep = 1
init_rep = 10

MAX_CUR = 5.0
MIN_CUR = 0.0
Dcur = 1
init_cur = 15

MAX_RAMP = 3
MIN_RAMP = 1
Dramp = 1
init_ramp = 3

MAX_FQ = 50
MIN_FQ = 20
Dfq = 5
init_fq = 30
# This is just some index value
i_rep = 3
i_cur = 2
i_ramp = 1
i_status = 4
i_fq = 5
i_cnt = 6

# Display lists: stimulator channel, exercise, method and status
global ch_select, ch_str
ch_select = ["Ch.Red", "Ch.Blue", "Ch.Black", "Ch.White"]
global ex_select, ex_str, ex_current
ex_select = ["Lower leg flexion", "Upper leg extension", "Other"]
global status_list
status_list = ["Initialization", "Setting threshold", "EMG monitoring", "Tigger FES", "Stop FES", "Finish program", "Manual calibration", "Automatic calibration\n- stimulating", "Automatic calibration\n- resting", "Exercise finished"]
global mth_select, mth_str, mth_current
mth_select = ["Threshold 1 (SD*0.5)", "Threshold 2 (SD*0.3)", "Threshold 3 (other)"]

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

# fonts sizes
fSize_cal = 13
bSize_w_cal = 10
bSize_h_cal = 3

# ========================== Functions ==========================
#
# ========================== Window layout ==========================
ctrl_left_color = LSR_TEXT
ctrl_left_fg = 'white'
ctr_mid_color = 'white'
ctr_mid_fg = 'black'
ctr_right_color = 'green'

# Sizes
total_width = 640
total_height = 640
top_height = 100
btm_height = 60
middle_height = total_height-(3*top_height)

root = Tk()
root.title('GUI SAS')
root.geometry('{}x{}'.format(total_width, total_height))

# create all of the main containers
top_frame = Frame(root, bg=ctr_mid_color, width=total_width, height=top_height, pady=1)
center = Frame(root, bg='gray2', width=total_width, height=middle_height, pady=1)
btm_frame = Frame(root, bg=LSR_TEXT, width=total_width, height=top_height, pady=1)
btm_frame2 = Frame(root, bg='lavender', width=total_width, height=top_height, pady=1)

# layout all of the main containers
root.grid_rowconfigure(1, weight=1)
root.grid_columnconfigure(0, weight=1)

top_frame.grid(row=0, sticky="ew")
center.grid(row=2, sticky="nsew")
btm_frame.grid(row=4, sticky="ew")
btm_frame2.grid(row=5, sticky="ew")

# create the center widgets
top_frame.grid_rowconfigure(0, weight=1)
top_frame.grid_columnconfigure(1, weight=1)

center.grid_rowconfigure(0, weight=1)
center.grid_columnconfigure(1, weight=1)

ctr_left = Frame(center, bg=ctrl_left_color, width=100, height=190)
ctr_mid_top = Frame(center, bg=ctr_mid_color, width=250, height=85, padx=3, pady=3)

ctr_left.grid(row=0, column=0, sticky="ew")
ctr_mid_top.grid(row=0, column=1, sticky="nsew")

# =================== Window decoration ===================
# the top frame
param_title_label = Label(top_frame, font=("Arial Bold", 15), bg=ctr_mid_color, fg=STANDARD_BLUE)

# bottom end
c_symbol = "\u00a9"
text_copy = 'Copyright:' + c_symbol + \
    "2020 Life Science Robotics " + '&' + " Aalborg University."
bottom_label = Label(btm_frame2, text=text_copy, bg='lavender', fg=AAU_BLUE)

# Counters for the buttons on the left
counters = [tk.DoubleVar() for _ in range(30)]

# =================== Center-top objects ===================
select_frame = top_frame
fSize = 17
bSize_w = 3
bSize_h = 1

status_value = tk.Label(select_frame, width = 20, font=("Arial Bold", fSize), bg=ctr_mid_color, fg=ctr_mid_fg, anchor="w")
status_display = tk.Label(select_frame, width = 60, height = 3, font=("Arial Bold", 10), bg=ctr_mid_color, fg=ctr_mid_fg, anchor="nw")

# =================== Center description ===================
select_frame = ctr_mid_top

frame_title = Label(select_frame, text='Exercise settings', fg=STANDARD_BLUE,
                     font=("Arial Bold", 20), bg=ctr_mid_color)
# Start training / set threshold
thButton =  tk.Button(select_frame, text='Set\nthreshold', font=("Arial Bold", fSize), width=10, height=2, bg=LSR_LINE, fg=ctrl_left_fg)
trainButton = tk.Button(select_frame, text='Start\ntraining', font=("Arial Bold", fSize), width=10, height=2, bg=LSR_PLUS, fg=ctrl_left_fg)

# Exercise display and selection
ex_current = Label(select_frame, text='Current exercise:',
                     font=("Arial Bold", 10), bg=ctr_mid_color, fg = ctr_mid_fg)

exButton_repeat = tk.Button(select_frame, font=("Arial Bold", fSize), width=10,
                           height=2, bg=STANDARD_BLUE, fg=ctrl_left_fg)

exButton_new = tk.Button(select_frame, font=("Arial Bold", fSize), width=10,
                           height=2, bg=STANDARD_BLUE, fg=ctrl_left_fg)


ex_next = Label(select_frame, text='Next exercise:',
                     font=("Arial Bold", 10), bg=ctr_mid_color, fg = ctr_mid_fg)

# Select exercise
ex_str = StringVar(root)
ex_str.set(ex_select[EX_LOWERLEG])
ex_box = OptionMenu(select_frame, ex_str, *ex_select)
ex_box.configure(width=20, font=("Arial Bold", 10),
                 bg=ctr_mid_color, fg=ctr_mid_fg)

# Method display and selection
mth_current = Label(select_frame, text='Current method: ',
                     font=("Arial Bold", 10), bg=ctr_mid_color, fg = ctr_mid_fg)

mth_next = Label(select_frame, text='For next exercise:',
                     font=("Arial Bold", 10), bg=ctr_mid_color, fg = ctr_mid_fg)

# Select method
mth_str = StringVar(root)
mth_str.set(mth_select[th_SD05])
mth_box = OptionMenu(select_frame, mth_str, *mth_select)
mth_box.configure(width=20, font=("Arial Bold", 10),
                 bg=ctr_mid_color, fg=ctr_mid_fg)

# =================== Side buttons description ===================
select_frame = ctr_left
# Defining buttons size and their font
fSize = 20
bSize_w = 3
bSize_h = 1

stim_label = tk.Label(select_frame, font=("Arial Bold", fSize+2), bg=ctrl_left_color, fg=ctrl_left_fg)

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

cur_value = tk.Label(select_frame, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg)
curButton_plus = tk.Button(select_frame, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h,  text="+", bg=STANDARD_BLUE, fg=ctrl_left_fg)
curButton_minus = tk.Button(select_frame, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h,  text="-", bg=STANDARD_BLUE, fg=ctrl_left_fg)

# Frequency options
fq_label = tk.Label(select_frame, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg)
fq_value = tk.Label(select_frame, font=("Arial Bold", fSize), bg=ctrl_left_color, fg=ctrl_left_fg)
fqButton_plus = tk.Button(select_frame, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h,  text="+", bg=STANDARD_BLUE, fg=ctrl_left_fg)
fqButton_minus = tk.Button(select_frame, font=("Arial Bold", fSize), width=bSize_w, height=bSize_h,  text="-", bg=STANDARD_BLUE, fg=ctrl_left_fg)


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


dummy_label1 = Label(select_frame, text='      ',
                     font=("Arial Bold", 16), bg=dummy_color)

manButton = tk.Button(select_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, fg=LSR_TEXT, bg=ctrl_left_fg)
autoButton = tk.Button(select_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Automatic\nCalibration", fg=LSR_TEXT, bg=ctrl_left_fg)
xButton = tk.Button(select_frame, font=("Arial", fSize_cal), width=bSize_w_cal, height=bSize_h_cal, text="Quit\nCalibration", fg=LSR_TEXT, bg=ctrl_left_fg)

dummy_label2 = Label(select_frame, text='               ', font=(
    "Arial Bold", 16), bg=dummy_color)


#  ===== Options that have been taken out from the main script due to R&D requirements: ============

# Increase/decrease ramp
#rampButton_plus.grid(row=3, column=2)
#rampButton_minus.grid(row=3, column=0)

# Increase/decrease repetitions
#repButton_plus ['command']= lambda counter = counters[i_rep]: increase_stat(counter=counter, incr=Drep, limit=MAX_REP, type=i_rep)
#repButton_minus ['command']= lambda counter=counters[i_rep]: decrease_stat(counter=counter, incr=Drep, limit=MIN_REP, type=i_rep)
#repButton_plus.grid(row=rnum+1, column=2)
#repButton_minus.grid(row=rnum+1, column=0)

# Enable / disable stimulator channels:
#enButton['command'] =lambda: user_button(type=(Move3_en_ch+10))
#enButton.grid(row=rnum+1, column=1)
# (from update_display())
#    if (ch_active == 1.0):
#        enButton['text'] = "Disable\nchannel"
#    else:
#        enButton['text'] = "Enable\nchannel"
#    if(active == 0 and ch_active == 0):
#        enButton['bg'] = LSR_PLUS
#    else:
#        enButton['bg'] = AAU_BLUE

# Stimulation  multi-channel selection 
# ch_box.grid(row=rnum, column=1)
# (from update_display())
    # Select boxes and options while manual calibration is active
#    if(status == st_calM):
#        ch_box.configure(state="normal")
#        enButton.configure(state="normal")
#    else:
#        ch_str.set(ch_select[CH_RED])
#        ch_box.configure(state="disabled")
#        enButton.configure(state="disabled")

# Automatic calibration
#autoButton['command'] = lambda: user_button(type=User_CA)
#autoButton.grid(row=rnum, column=16)

