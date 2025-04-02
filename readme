# Dexter Project

Dexter is a Windows-based utility designed to provide process monitoring, network connection monitoring, and basic firewall management. It also includes features for managing whitelists and blacklists, as well as executing system commands.

## Features

- **Process Monitoring**: Displays a list of all active processes on the system.
- **Network Monitoring**: Shows active network connections using `netstat`.
- **Firewall Management**:
  - Block IPs from a blacklist.
  - Unblock previously blocked IPs.
- **Whitelist/Blacklist Management**:
  - Add IPs to a whitelist or blacklist.
- **Malware Verification**: Scans the system and generates a file list for manual inspection.
- **Command Execution**: Allows execution of custom commands like `neofetch`.

## Requirements

- Windows OS
- Administrator privileges (required for firewall and network monitoring features)
- `netstat` command available in the system PATH

## How to Use

1. **Compile the Project**:
   - Use a C compiler like `gcc` (MinGW for Windows).
   - Example: `gcc dexter.c -o dexter.exe`

2. **Run the Program**:
   - Open a terminal with administrator privileges.
   - Execute the compiled binary: `dexter.exe`.

3. **Menu Options**:
   - The program provides a menu with the following options:
     1. Execute `neofetch` in CMD.
     2. Monitor active processes.
     3. Monitor network connections.
     4. Verify for malware.
     5. Add an IP to the whitelist.
     6. Add an IP to the blacklist.
     7. Block IPs from the blacklist.
     8. Unblock all blocked IPs.
     9. Exit the program.

4. **Log Display**:
   - The program maintains a log of the last actions performed and their results, displayed on the main menu.

## File Structure

- `dexter.c`: The main source code file.
- `blacklist.txt`: A text file containing IPs to be blocked.
- `whitelist.txt`: A text file containing IPs to be whitelisted.
- `file_list.txt`: A generated file containing a list of all files on the system (used for malware verification).

## Notes

- **Administrator Privileges**: Some features, such as blocking/unblocking IPs and monitoring network connections, require the program to be run as an administrator.
- **Firewall Rules**: The program uses Windows Firewall to block/unblock IPs. Ensure that the firewall is enabled for these features to work.
- **Log Size**: The log displayed in the menu is limited in size. If the log becomes too large, older entries may be truncated.

## Example Usage

1. **Monitor Processes**:
   - Select option `2` from the menu to display a list of active processes.

2. **Block IPs**:
   - Add IPs to `blacklist.txt`.
   - Select option `7` to block the listed IPs.

3. **Monitor Network Connections**:
   - Select option `3` to display active network connections.

4. **Verify Malware**:
   - Select option `4` to generate a file list for manual inspection.

## License

This project is open-source and available under the MIT License.

## Disclaimer

This tool is intended for educational purposes and personal use only. Use it responsibly and ensure compliance with local laws and regulations.