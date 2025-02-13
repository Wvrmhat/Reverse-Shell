# Reverse-Shell

A reverse shell is the use of a server to execute commands on a compromised machine **(client)** and receive the output. This is done by establishing a TCP connection where the client connects to the server/attacker and awaits commands. This program works only on windows machines as it uses the Windows Sockets API.

## Use Cases
 - Penetration testers can use this tool to assess how well a system can defend itself against unauthorized remote access. This falls under red teaming.
 - A system admin could modify the tool for legitimate remote system management. Commands can then be executed remotely without physically accessing the machine.

>[!Warning]
> This repository contains penetration testing tools developed in C++. These tools are intended strictly for educational and ethical hacking purposes. They should only be used in environments where you have explicit permission to test. I am not responsible for any misuse, damage, or legal issues arising from the use of this program. It is the responsibility of the user to comply with all applicable laws and regulations.
