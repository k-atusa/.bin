# test776 : YAS gui

import time
import re
import zlib
import base64

import threading
from collections import deque

import sys
import os
import subprocess

import tkinter as tk
import tkinter.ttk as ttk
import tkinter.messagebox as tkm
from tkinter import filedialog as tkf

# pyinstaller -w -F main.py

class yas:
    def __init__(self, os_type):
        # basic configure
        self.windows = False
        if os_type == "win":
            self.parms = {"wsize":"500x700", "font_btn":("Helvetica", 12), "font_lbl":("Helvetica", 12), "input_h":12, "text_h":6}
            self.windows = True
        elif os_type == "mac":
            self.parms = {"wsize":"800x1120", "font_btn":("Helvetica", 12), "font_lbl":("Helvetica", 12), "input_h":12, "text_h":6}
        elif os_type == "linux":
            self.parms = {"wsize":"800x1120", "font_btn":("Helvetica", 12), "font_lbl":("Helvetica", 12), "input_h":12, "text_h":6}
        else:
            self.parms = {"wsize":"500x700", "font_btn":("Helvetica", 12), "font_lbl":("Helvetica", 12), "input_h":12, "text_h":6}

        # data setup
        self.me_public, self.me_private, self.you_name, self.yaspath = "", "", [ ], ""
        self.flags = [True, False, True, False, False, False] # sign debug rsa edit ext_yas ext_key
        self.files, self.inputdata = [ ], "" # input datas

    def loadinfo(self):
        # load yaspath
        self.yaspath = os.path.abspath(sys.argv[0]).replace("\\", "/")
        self.yaspath = self.yaspath[:self.yaspath.rfind("/")+1]
        self.flags[4] = os.path.exists(self.yaspath + "yas_cli.exe") or os.path.exists(self.yaspath + "yas_cli")
        self.flags[5] = os.path.exists(self.yaspath + "public.txt") and os.path.exists(self.yaspath + "private.txt")
        if not os.path.exists(self.yaspath + "address"):
            os.mkdir(self.yaspath + "address")

        # load key
        if self.flags[5]:
            with open(self.yaspath + "public.txt", "r", encoding="utf-8") as f:
                self.me_public = f.read()
            self.me_public = "\n".join(self.me_public[i:i+80] for i in range(0, len(self.me_public), 80))
            with open(self.yaspath + "private.txt", "r", encoding="utf-8") as f:
                self.me_private = f.read()
            self.me_private = "\n".join(self.me_private[i:i+80] for i in range(0, len(self.me_private), 80))
        self.you_name = os.listdir(self.yaspath + "address")
        self.you_name.sort()

    def render(self):
        # window and style
        self.win = tk.Tk()
        self.win.title("YAS-gui")
        self.win.geometry( self.parms["wsize"] )
        self.win.resizable(1, 1)
        style = ttk.Style()
        style.configure( "TLabelframe.Label", font=self.parms["font_lbl"] )
        style.configure( "TButton", font=self.parms["font_btn"] )
        style.configure( "TCheckbutton", font=self.parms["font_btn"] )

        # notebook pages
        self.notebook = ttk.Notebook(self.win, padding=5)
        self.notebook.pack()
        self.frame_run = ttk.Frame(self.win)
        self.notebook.add(self.frame_run, text="  Program  ")
        self.frame_input = tk.Frame(self.win)
        self.notebook.add(self.frame_input, text="   Input   ")
        self.frame_profile = ttk.Frame(self.win)
        self.notebook.add(self.frame_profile, text="  Profile  ")

        # start loop
        self.draw_run()
        self.draw_console()
        self.draw_input()
        self.draw_profile()
        if not self.flags[4]:
            tkm.showerror("No YAS-cli file", " yas-cli binary not detected. ")
        if not self.flags[5]:
            tkm.showinfo("No Host Key", " no private & public key detected. ")
        self.win.mainloop()

    def draw_run(self):
        # run config & input
        uframe = ttk.Labelframe(self.frame_run, text="  Run  ", padding=(10, 15))
        uframe.pack(fill="x", padx=10, pady=10)
        ttk.Label(uframe, text="input", font=self.parms["font_lbl"]).grid(row=0, column=0, sticky="w", padx=5, pady=5)
        self.svar0 = tk.StringVar()
        self.input0 = ttk.Entry(uframe, font=self.parms["font_lbl"], state="readonly", textvariable=self.svar0)
        self.input0.grid(row=0, column=1, columnspan=3, sticky="we", padx=5, pady=5)

        # password & message
        ttk.Label(uframe, text="password", font=self.parms["font_lbl"]).grid(row=1, column=0, sticky="w", padx=5, pady=5)
        self.input1 = ttk.Entry(uframe, font=self.parms["font_lbl"], show="*")
        self.input1.grid(row=1, column=1, columnspan=3, sticky="we", padx=5, pady=5)
        ttk.Label(uframe, text="message", font=self.parms["font_lbl"]).grid(row=2, column=0, sticky="w", padx=5, pady=5)
        self.input3 = ttk.Entry(uframe, font=self.parms["font_lbl"])
        self.input3.grid(row=2, column=1, columnspan=3, sticky="we", padx=5, pady=5)

        # receiver, checkbuttons
        ttk.Label(uframe, text="receiver", font=self.parms["font_lbl"]).grid(row=3, column=0, sticky="w", padx=5, pady=5)
        self.combo4 = ttk.Combobox(uframe, font=self.parms["font_lbl"], values=self.you_name, state="readonly")
        self.combo4.grid(row=3, column=1, sticky="we", padx=5, pady=5)
        tempframe = ttk.Frame(uframe)
        tempframe.grid(row=3, column=2, columnspan=2, sticky="w", padx=5, pady=5)
        self.ivar5 = tk.IntVar()
        self.ivar5.set(self.flags[0])
        self.check5 = ttk.Checkbutton(tempframe, text="sign", variable=self.ivar5, command=self.f0)
        self.check5.pack(side="left", padx=5, pady=5)
        self.ivar6 = tk.IntVar()
        self.ivar6.set(self.flags[1])
        self.check6 = ttk.Checkbutton(tempframe, text="debug", variable=self.ivar6, command=self.f1)
        self.check6.pack(side="left", padx=5, pady=5)
        
        # modes, run button
        tempmode = ["1) manual file encryption", "2) manual file decryption", "3) automatic send", "4) automatic receive",
                     "5) pgp text encryption", "6) pgp text decryption", "7) pgp data encryption", "8) pgp data decryption"]
        ttk.Label(uframe, text="mode", font=self.parms["font_lbl"]).grid(row=4, column=0, sticky="w", padx=5, pady=5)
        self.combo7 = ttk.Combobox(uframe, font=self.parms["font_lbl"], values=tempmode, state="readonly")
        self.combo7.set(tempmode[0])
        self.combo7.grid(row=4, column=1, sticky="we", padx=5, pady=5)
        self.button8 = ttk.Button(uframe, text="run", command=self.run)
        self.button8.grid(row=4, column=2, sticky="we", padx=5, pady=5)

    def draw_console(self):
        # console textview
        dframe = ttk.Labelframe(self.frame_run, text="  Console  ", padding=(5, 5))
        dframe.pack(fill="x", padx=10, pady=10)
        tempbar = ttk.Scrollbar(dframe)
        tempbar.pack(side="right", fill="y")
        self.text9 = tk.Text(dframe, state="disabled", font=self.parms["font_lbl"])
        self.text9.pack(fill="x", padx=5, pady=5)
        tempbar["command"] = self.text9.yview

    def draw_input(self):
        # ip port input
        uframe = ttk.Labelframe(self.frame_input, text="  Sender  ", padding=(5, 5))
        uframe.pack(fill="x", padx=10, pady=10)
        ttk.Label(uframe, text="ip:port", font=self.parms["font_lbl"]).grid(row=0, column=0, sticky="w", padx=5, pady=5)
        self.input10 = ttk.Entry(uframe, font=self.parms["font_lbl"])
        self.input10.grid(row=0, column=1, sticky="w", padx=5, pady=5)
        self.button11 = ttk.Button(uframe, text="submit", command=self.f2)
        self.button11.grid(row=0, column=2, sticky="we", padx=5, pady=5)

        # text input
        mframe = ttk.Labelframe(self.frame_input, text="  Text  ", padding=(5, 5))
        mframe.pack(fill="x", padx=10, pady=10)
        self.button12 = ttk.Button(mframe, text="submit", command=self.f3)
        self.button12.pack(side="top", anchor="w", padx=5, pady=5)
        tbar0 = ttk.Scrollbar(mframe)
        tbar0.pack(side="right", fill="y")
        self.text13 = tk.Text(mframe, font=self.parms["font_lbl"], height=self.parms["input_h"])
        self.text13.pack(fill="x", padx=5, pady=5)
        tbar0["command"] = self.text13.yview

        # file input frame
        dframe = ttk.Labelframe(self.frame_input, text="  File  ", padding=(5, 5))
        dframe.pack(fill="x", padx=10, pady=10)
        tempframe = ttk.Frame(dframe)
        tempframe.pack(side="left", fill="y")

        # file input buttons
        self.button14 = ttk.Button(tempframe, text="submit", command=self.f4)
        self.button14.pack(side="top", padx=5, pady=5)
        self.button15 = ttk.Button(tempframe, text="add file", command=self.f5)
        self.button15.pack(side="top", padx=5, pady=5)
        self.button16 = ttk.Button(tempframe, text="add dir", command=self.f6)
        self.button16.pack(side="top", padx=5, pady=5)
        self.button17 = ttk.Button(tempframe, text="delete", command=self.f7)
        self.button17.pack(side="top", padx=5, pady=5)

        # file input list
        tbar1 = ttk.Scrollbar(dframe)
        tbar1.pack(side="right", fill="y")
        self.list18 = tk.Listbox(dframe, font=self.parms["font_lbl"])
        self.list18.pack(fill="x", padx=5, pady=5)
        tbar1["command"] = self.list18.yview

    def draw_profile(self):
        # my key frame
        uframe = ttk.Labelframe(self.frame_profile, text="  My Key  ", padding=(5, 5))
        uframe.pack(fill="x", padx=10, pady=10)
        tframe0 = ttk.Frame(uframe)
        tframe0.pack(side="top", anchor="w")

        # my key buttons
        self.button19 = ttk.Button(tframe0, text="regenerate", command=self.f8)
        self.button19.grid(row=0, column=0, sticky="w", padx=5, pady=5)
        self.ivar20 = tk.IntVar()
        self.ivar20.set(self.flags[2])
        self.check20 = ttk.Checkbutton(tframe0, text="rsa-4096", variable=self.ivar20, command=self.f9)
        self.check20.grid(row=0, column=1, sticky="w", padx=5, pady=5)
        self.button21 = ttk.Button(tframe0, text="update", command=self.f10)
        self.button21.grid(row=0, column=2, sticky="w", padx=5, pady=5)
        self.ivar22 = tk.IntVar()
        self.ivar22.set(self.flags[3])
        self.check22 = ttk.Checkbutton(tframe0, text="edit", variable=self.ivar22, command=self.f11)
        self.check22.grid(row=0, column=3, sticky="w", padx=5, pady=5)

        # textview, set editable
        ttk.Label(uframe, text="my public key", font=self.parms["font_lbl"]).pack(side="top", anchor="w", padx=5, pady=5)
        self.text23 = tk.Text(uframe, font=self.parms["font_lbl"], height=self.parms["text_h"])
        self.text23.pack(side="top", fill="x", padx=5, pady=5)
        ttk.Label(uframe, text="my private key", font=self.parms["font_lbl"]).pack(side="top", anchor="w", padx=5, pady=5)
        self.text24 = tk.Text(uframe, font=self.parms["font_lbl"], height=self.parms["text_h"])
        self.text24.pack(side="top", fill="x", padx=5, pady=5)
        tempstate = "normal" if self.flags[3] else "disabled"
        self.text23.insert("1.0", self.me_public)
        self.text24.insert("1.0", self.me_private)
        self.text23.config(state=tempstate)
        self.text24.config(state=tempstate)

        # address frame
        dframe = ttk.Labelframe(self.frame_profile, text="  Address List  ", padding=(5, 5))
        dframe.pack(fill="x", padx=10, pady=10)
        tframe1 = ttk.Frame(dframe)
        tframe1.pack(side="left", fill="y")

        # address buttons
        self.button25 = ttk.Button(tframe1, text="add key", command=self.f12)
        self.button25.pack(side="top", padx=5, pady=5)
        self.button26 = ttk.Button(tframe1, text="view key", command=self.f13)
        self.button26.pack(side="top", padx=5, pady=5)
        self.button27 = ttk.Button(tframe1, text="delete", command=self.f14)
        self.button27.pack(side="top", padx=5, pady=5)
        self.button28 = ttk.Button(tframe1, text="sync list", command=self.f15)
        self.button28.pack(side="top", padx=5, pady=5)

        # address list
        tbar = ttk.Scrollbar(dframe)
        tbar.pack(side="right", fill="y")
        self.list29 = tk.Listbox(dframe, font=self.parms["font_lbl"])
        self.list29.pack(fill="x", padx=5, pady=5)
        tbar["command"] = self.list29.yview
        for i in self.you_name:
            self.list29.insert(tk.END, i)

    def f0(self): # run_sign
        time.sleep(0.1)
        self.flags[0] = not self.flags[0]

    def f1(self): # run_debug
        time.sleep(0.1)
        self.flags[1] = not self.flags[1]

    def f2(self): # input_submit_ip
        time.sleep(0.1)
        temp = self.input10.get()
        if re.match("^(\\d{1,3}\\.){3}\\d{1,3}:\\d{1,5}$", temp):
            self.inputdata, self.files = temp, [ ]
            self.svar0.set(f"-i {self.inputdata}")
        else:
            tkm.showwarning("Invalid IP", f" {temp} does not match N.N.N.N:P ")

    def f3(self): # input_submit_text
        time.sleep(0.1)
        self.inputdata, self.files = self.text13.get("1.0", tk.END)[:-1], [ ]
        temp = f"-i \"{self.inputdata}\""
        if len(temp) > 160:
            temp = temp[:160] + "..."
        self.svar0.set(temp.replace("\n", " "))

    def f4(self): # input_submit_file
        time.sleep(0.1)
        self.inputdata, temp = "", ""
        for i in self.files:
            temp = temp + f"-i \"{i}\" "
        if len(temp) > 160:
            temp = temp[:160] + "..."
        self.svar0.set(temp)

    def f5(self): # input_addfile
        time.sleep(0.1)
        for i in tkf.askopenfiles(title="Select Files"):
            i = os.path.abspath(i.name).replace("\\", "/")
            self.files.append(i)
            self.list18.insert(tk.END, i)

    def f6(self): # input_adddir
        time.sleep(0.1)
        i = os.path.abspath(tkf.askdirectory(title="Select Directory")).replace("\\", "/")
        self.files.append(i)
        self.list18.insert(tk.END, i)

    def f7(self): # input_delete
        time.sleep(0.1)
        self.files = [ ]
        self.list18.delete(0, tk.END)

    def f8(self): # profile_regen
        time.sleep(0.1)
        args = [self.yaspath+"yas_cli", "-pk", "-o", self.yaspath]
        if self.windows:
            args[0] = args[0].replace("/", "\\") + ".exe"
        if self.flags[2]:
            args.append("-sign")

        # run program
        if tkm.askokcancel("Regenerate Key?", " This work will delete existing key. "):
            tkm.showinfo("Key Generated", subprocess.run(args, text=True, capture_output=True).stdout)
            with open(self.yaspath + "public.txt", "r", encoding="utf-8") as f:
                self.me_public = f.read()
            self.me_public = "\n".join(self.me_public[i:i+80] for i in range(0, len(self.me_public), 80))
            with open(self.yaspath + "private.txt", "r", encoding="utf-8") as f:
                self.me_private = f.read()
            self.me_private = "\n".join(self.me_private[i:i+80] for i in range(0, len(self.me_private), 80))

            # update profile
            self.text23.config(state="normal")
            self.text24.config(state="normal")
            self.text23.delete("1.0", tk.END)
            self.text24.delete("1.0", tk.END)
            self.text23.insert("1.0", self.me_public)
            self.text24.insert("1.0", self.me_private)
            tempstate = "normal" if self.flags[3] else "disabled"
            self.text23.config(state=tempstate)
            self.text24.config(state=tempstate)

    def f9(self): # profile_rsa
        time.sleep(0.1)
        self.flags[2] = not self.flags[2]

    def f10(self): # profile_update
        time.sleep(0.1)
        if self.flags[3]:
            if tkm.askokcancel("Update Key?", " This work will delete existing key. "):
                self.me_public = self.text23.get("1.0", tk.END)
                with open(self.yaspath + "public.txt", "w", encoding="utf-8") as f:
                    f.write(self.me_public)
                self.me_private = self.text24.get("1.0", tk.END)
                with open(self.yaspath + "private.txt", "w", encoding="utf-8") as f:
                    f.write(self.me_private)

    def f11(self): # profile_edit
        time.sleep(0.1)
        self.flags[3] = not self.flags[3]
        tempstate = "normal" if self.flags[3] else "disabled"
        self.text23.config(state=tempstate)
        self.text24.config(state=tempstate)

    def f12(self): # profile_addkey
        time.sleep(0.1)
        keypath = tkf.askopenfile(title="Select Public Key", filetypes=[("text files","*.txt"),("all files","*.*")]).name.replace("\\", "/")
        keyname = keypath[keypath.rfind("/")+1:]
        if keyname in self.you_name:
            tkm.showinfo("Existing Address", f" {keyname} will be overwrited. ")
        else:
            self.you_name.append(keyname)
            self.list29.insert(tk.END, keyname)
        with open(keypath, "r", encoding="utf-8") as f:
            with open(f"{self.yaspath}address/{keyname}", "w",encoding="utf-8") as t:
                t.write(f.read())

    def f13(self): # profile_viewkey
        time.sleep(0.1)
        keyname = self.you_name[ self.list29.curselection()[0] ]
        with open(f"{self.yaspath}address/{keyname}", "r", encoding="utf-8") as f:
            keydata = f.read()
        crc = zlib.crc32( base64.b64decode( keydata.replace(" ", "").replace("\n", "") ) ) & 0xFFFFFFFF
        tkm.showinfo(f"{keyname} ({crc:08x})", keydata)

    def f14(self): # profile_delete
        time.sleep(0.1)
        pos = self.list29.curselection()[0]
        keyname = self.you_name[pos]
        if tkm.askokcancel("Key Delete", f" Delete public key {keyname} ? "):
            os.remove(f"{self.yaspath}address/{keyname}")
            self.you_name.pop(pos)
            self.list29.delete(pos, pos)

    def f15(self): # profile_sync
        time.sleep(0.1)
        self.you_name = os.listdir(self.yaspath + "address")
        self.you_name.sort()
        self.combo4.config(values=self.you_name)
        self.list29.delete(0, tk.END)
        for i in self.you_name:
            self.list29.insert(tk.END, i)

    def run(self): # run_run
        time.sleep(0.1)
        if self.windows:
            self.button8.busy()
        else:
            self.button8.config(state="disabled")
        self.text9.config(state="normal")
        self.text9.delete("1.0", tk.END)

        # make command
        command = [f"{self.yaspath}yas_cli"]
        if self.windows:
            command[0] = command[0].replace('/','\\') + ".exe"
        if self.flags[0]:
            command.append("-sign")
        if self.flags[1]:
            command.append("-debug")
        mode = self.combo7.get()[0]

        if mode == "1": # manual file encryption
            command.append("-e")
            for i in self.files:
                command.append("-i")
                command.append(i)
            command.append("-pw")
            command.append(self.input1.get())
            command.append("-msg")
            command.append(self.input3.get())

        elif mode == "2": # manual file decryption
            command.append("-d")
            for i in self.files:
                command.append("-i")
                command.append(i)
            command.append("-pw")
            command.append(self.input1.get())

        elif mode == "3": # automatic send
            command.append("-s")
            for i in self.files:
                command.append("-i")
                command.append(i)

        elif mode == "4": # automatic receive
            command.append("-r")
            command.append("-i")
            command.append(self.inputdata)

        elif mode == "5": # pgp text encryption
            command.append("-pe")
            command.append("-i")
            command.append(self.inputdata)
            if self.flags[0]:
                command.append("-me")
                command.append(f"{self.yaspath}private.txt")
            temp = self.combo4.get()
            if temp != "":
                command.append("-you")
                command.append(f"{self.yaspath}address/{temp}")

        elif mode == "6": # pgp text decryption
            command.append("-pd")
            command.append("-i")
            command.append(self.inputdata)
            command.append("-me")
            command.append(f"{self.yaspath}private.txt")
            temp = self.combo4.get()
            if temp != "":
                command.append("-you")
                command.append(f"{self.yaspath}address/{temp}")

        elif mode == "7": # pgp data encryption
            command.append("-ps")
            for i in self.files:
                command.append("-i")
                command.append(i)
            if self.flags[0]:
                command.append("-me")
                command.append(f"{self.yaspath}private.txt")
            temp = self.combo4.get()
            if temp != "":
                command.append("-you")
                command.append(f"{self.yaspath}address/{temp}")

        elif mode == "8": # pgp data decryption
            command.append("-pr")
            for i in self.files:
                command.append("-i")
                command.append(i)
            command.append("-me")
            command.append(f"{self.yaspath}private.txt")
            temp = self.combo4.get()
            if temp != "":
                command.append("-you")
                command.append(f"{self.yaspath}address/{temp}")
        
        # run process
        queue = deque()
        t = threading.Thread(target=self.run_sub, args=(command, queue))
        t.start()
        while t.is_alive():
            time.sleep(0.1)
            self.win.update()
            while len(queue) != 0:
                self.text9.insert(tk.END, queue.popleft())

        # end process
        t.join()
        if self.windows:
            self.button8.busy_forget()
        else:
            self.button8.config(state="normal")
        self.text9.config(state="disabled")

    def run_sub(self, command, queue): # run & update
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1, encoding="utf-8")
        for line in process.stdout:
            queue.append(line)
        process.stdout.close()
        process.wait()

k = yas("win")
k.loadinfo()
k.render()
