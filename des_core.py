from Crypto.Cipher import DES
from Crypto.Util.Padding import pad, unpad
import binascii

def encrypt_message(key: str, plaintext: str) -> str:
    """Encrypts a string using DES (ECB mode) and returns a HEX string."""
    # DES strictly requires an 8-byte (64-bit) key
    if len(key) != 8:
        raise ValueError("DES key must be exactly 8 characters.")
    
    cipher = DES.new(key.encode('utf-8'), DES.MODE_ECB)
    
    # Pad the text so its length is a multiple of the DES block size (8 bytes)
    padded_data = pad(plaintext.encode('utf-8'), DES.block_size)
    encrypted_bytes = cipher.encrypt(padded_data)
    
    # Return as an uppercase HEX string for easy copying/pasting
    return binascii.hexlify(encrypted_bytes).decode('utf-8').upper()

def decrypt_message(key: str, ciphertext_hex: str) -> str:
    """Decrypts a HEX string using DES (ECB mode) and returns the plaintext."""
    if len(key) != 8:
        raise ValueError("DES key must be exactly 8 characters.")
    
    cipher = DES.new(key.encode('utf-8'), DES.MODE_ECB)
    
    # Convert HEX back to bytes
    encrypted_bytes = binascii.unhexlify(ciphertext_hex)
    
    # Decrypt and remove padding
    decrypted_padded_bytes = cipher.decrypt(encrypted_bytes)
    decrypted_data = unpad(decrypted_padded_bytes, DES.block_size)
    
    return decrypted_data.decode('utf-8')
