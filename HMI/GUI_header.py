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

# Stimulator multichannel list
global ch_select, ch_str
ch_select = ["Ch.Red", "Ch.Blue", "Ch.Black", "Ch.White"]

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

# ========================== Functions ==========================

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
# ctr_right = Frame(center, bg=ctr_right_color, width=100, height=190, padx=3, pady=3)

ctr_left.grid(row=0, column=0, sticky="ns")
ctr_mid.grid(row=0, column=1, sticky="nsew")
# ctr_right.grid(row=0, column=2, sticky="ns")

# =================== Window decoration ===================
# the top frame
canvas = Canvas(root, width=1000, height=100)
img_banner = ImageTk.PhotoImage(Image.open("banner.jpg"))
banner_label = Label(top_frame, image=img_banner, bg='black')

# center middle
param_title_label = Label(ctr_mid, text=' Stimulation parameters',
                          font=("Arial Italic", 34), bg=ctr_mid_color, fg=AAU_BLUE)

canvas = Canvas(root, width=300, height=300)
img_param = ImageTk.PhotoImage(Image.open("HMI_diag1.jpg"))
param_label = Label(ctr_mid, image=img_param)

# bottom end
c_symbol = "\u00a9"
text_copy = 'Copyright:' + c_symbol + \
    "2020 Life Science Robotics " + '&' + " Aalborg University."
bottom_label = Label(btm_frame2, text=text_copy, bg='lavender', fg=AAU_BLUE)

# =================== Side buttons ===================
# Buttons on the blue side
counters = [tk.IntVar() for _ in range(30)]
# Defining buttons size and their font
fSize = 20
bSize_w = 3
bSize_h = 1

# Select stimulator channel
ch_str = StringVar(root)
ch_str.set(ch_select[CH_RED])
ch_box = OptionMenu(ctr_left, ch_str, *ch_select)
ch_box.configure(width=6, font=("Arial Bold", 20),
                 bg=ctrl_left_color, fg=ctrl_left_fg)