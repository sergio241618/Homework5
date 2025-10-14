# sniff_caesar.py
from scapy.all import sniff, TCP, Raw
import sys

def _rot_alpha(ch, shift):
    if 'a' <= ch <= 'z':
        base = ord('a')
        return chr((ord(ch) - base + shift) % 26 + base)
    if 'A' <= ch <= 'Z':
        base = ord('A')
        return chr((ord(ch) - base + shift) % 26 + base)
    return ch

def _rot_digit(ch, shift):
    if '0' <= ch <= '9':
        base = ord('0')
        return chr((ord(ch) - base + (shift % 10)) % 10 + base)
    return ch

def caesar_decrypt(payload: bytes) -> str:
    if not payload:
        return ""
    s = payload[0] % 26
    inv = (26 - s) % 26
    inv_d = (10 - (s % 10)) % 10
    text = payload[1:].decode('utf-8', errors='ignore')
    out = []
    for ch in text:
        if ch.isalpha():
            out.append(_rot_alpha(ch, inv))
        elif ch.isdigit():
            out.append(_rot_digit(ch, inv_d))
        else:
            out.append(ch)
    return "".join(out)

def handle(pkt):
    if pkt.haslayer(TCP) and pkt.haslayer(Raw):
        data = bytes(pkt[Raw].load)
        if len(data) >= 2:
            try:
                plain = caesar_decrypt(data)
                print(f"[+] Decrypted: {plain}")
            except Exception as e:
                print(f"[!] Decode error: {e}")

if __name__ == "__main__":
    iface = sys.argv[1] if len(sys.argv) > 1 else "wlo1" # Cambia esto a tu interfaz
    bpf = "tcp port 3333"
    print(f"Sniffing on {iface} with filter: {bpf}")
    sniff(iface=iface, filter=bpf, prn=handle, store=False)