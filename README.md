# Infestation


### Usermode

I've made simple usermode for testing, but i wont release, there are many sources available to copy and use.

### Kernelmode

<img src="./examples/test.png">
<a target="_blank" href="https://streamable.com/g937bg">Watch the demo</a>

The driver can be used in any windows 10/11 versions.

**Available**
- [x] Kernel driver with extra addons
- [x] kernel **mouse_movement** 
- [x] Kernel **mouse_down** and **mouse_up** 
- [x] Read virtual memory (**mmcopy**)
- [x] Write virtual memory (**mmcopy/mdl**)
- [x] Kernel driver ready to be manually mapped **tested** (kdmapper/lenovo)

**Planning**
- [ ] TODO: Kernel Dispatch **Write Memory** 
- [ ] TODO: Protect usermode process
- [ ] TODO: Clear MmUnloadedDrivers
- [ ] TODO: Clear PiDDBCacheTable

## How to use ?


### 1.1

- Clone the repository
- Open the solution in Visual Studio 2022 v143
- **DISABLE** `Security Check` if loading driver with driver mapper

<img src="./examples/security_check.png">

- Build the project
- Copy the driver to the same folder as the executable
- Load the driver manually with services or driver mapper


## Disclaimer ⚠

I wont support or provide any binaries for this project, you are free to use it as you wish, this is a free project.

