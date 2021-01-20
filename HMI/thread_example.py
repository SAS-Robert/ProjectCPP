import _thread
import time
from tkinter import *

# Interface objects definition
OPTIONS = [
"Jan",
"Feb",
"Mar"
] #etc

master = Tk()

variable = StringVar(master)
variable.set(OPTIONS[0]) # default value

w = OptionMenu(master, variable, *OPTIONS)
w.pack()

# Complementary functions 
def ok():
    print ("value is:" + variable.get())

# Thread functions definition 
def task():
    print("hello")
    master.after(2000, task)  # reschedule event in 2 seconds


def fn_update(delay):
   print ("value is:" + variable.get())
   time.sleep(delay)

def print_time( threadName, delay):
   count = 0
   while count < 5:
      time.sleep(delay)
      count += 1
      print ("%s: %s" % ( threadName, time.ctime(time.time()) ))

# Create two threads as follows
#try:
   #_thread.start_new_thread( print_time, ("Thread-1", 2, ) )
   #_thread.start_new_thread( fn_update, 4)
   #_thread.start_new_thread( fn_screen )
#except:
#   print ("Error: unable to start thread 1 ")

#while 1:
#   pass
master.after(2000, task)
master.mainloop()