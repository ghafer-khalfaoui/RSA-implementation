import customtkinter as ctk
from tkinter import messagebox
import des_core

# Configure the modern look
ctk.set_appearance_mode("dark")  # "dark", "light", or "system"
ctk.set_default_color_theme("blue")

class DESModernGUI(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.title("Modern DES Cryptography Tool")
        self.geometry("550x500")
        self.resizable(False, False)

        # Main Header
        self.lbl_title = ctk.CTkLabel(self, text="DES Encryption / Decryption", font=("Roboto", 24, "bold"))
        self.lbl_title.pack(pady=(20, 10))

        # Key Input Section
        self.lbl_key = ctk.CTkLabel(self, text="Secret Key (Exactly 8 characters):", font=("Roboto", 14))
        self.lbl_key.pack(pady=(10, 0))
        self.entry_key = ctk.CTkEntry(self, width=300, placeholder_text="e.g., 8bytekey")
        self.entry_key.pack(pady=(5, 10))

        # Input Message Section
        self.lbl_msg = ctk.CTkLabel(self, text="Message (Plaintext to encrypt / HEX to decrypt):", font=("Roboto", 14))
        self.lbl_msg.pack(pady=(10, 0))
        self.txt_msg = ctk.CTkTextbox(self, width=400, height=80)
        self.txt_msg.pack(pady=(5, 10))

        # Action Buttons Frame
        self.frame_btns = ctk.CTkFrame(self, fg_color="transparent")
        self.frame_btns.pack(pady=10)

        self.btn_enc = ctk.CTkButton(self.frame_btns, text="Encrypt", width=120, command=self.handle_encrypt)
        self.btn_enc.grid(row=0, column=0, padx=20)

        # Make the decrypt button red to differentiate it
        self.btn_dec = ctk.CTkButton(self.frame_btns, text="Decrypt", width=120, 
                                     fg_color="#C0392B", hover_color="#922B21", command=self.handle_decrypt)
        self.btn_dec.grid(row=0, column=1, padx=20)

        # Result Section
        self.lbl_res = ctk.CTkLabel(self, text="Result:", font=("Roboto", 14))
        self.lbl_res.pack(pady=(10, 0))
        self.txt_res = ctk.CTkTextbox(self, width=400, height=80)
        self.txt_res.pack(pady=(5, 20))

    def handle_encrypt(self):
        key = self.entry_key.get()
        msg = self.txt_msg.get("1.0", "end-1c").strip()
        try:
            result = des_core.encrypt_message(key, msg)
            self.txt_res.delete("1.0", "end")
            self.txt_res.insert("1.0", result)
        except Exception as e:
            messagebox.showerror("Encryption Error", str(e))

    def handle_decrypt(self):
        key = self.entry_key.get()
        msg = self.txt_msg.get("1.0", "end-1c").strip()
        try:
            result = des_core.decrypt_message(key, msg)
            self.txt_res.delete("1.0", "end")
            self.txt_res.insert("1.0", result)
        except Exception as e:
            messagebox.showerror("Decryption Error", str(e))

if __name__ == "__main__":
    app = DESModernGUI()
    app.mainloop()
