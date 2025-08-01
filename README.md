# MRO Image Downloader

A collection of tools to download CTX and HiRISE images from Mars Reconnaissance Orbiter (MRO) data archives.

## Prerequisites

### 1. Download Cumulative Index Files

Before using these tools, you need to download the appropriate cumulative index files and update the paths in the shell scripts.

#### CTX Images

- Download the latest `cumindex.tab` from: https://pds-imaging.jpl.nasa.gov/data/mro/ctx/
  - As of 7/30/2025, orbit #5100: https://planetarydata.jpl.nasa.gov/img/data/mro/ctx/mrox_5100/index/cumindex.tab
- Navigate to the most recent orbit folder and look in the `index` directory
- Update the `cumindex_loc` path in `code/download_ctx.s` (line 27)

#### HiRISE EDR Images (Raw)

- Download `EDRCUMINDEX.tab` from: https://hirise-pds.lpl.arizona.edu/PDS/INDEX/
- Update the `cumindex_loc` path in `code/download_hirise_edr.s` (line 28)

#### HiRISE RDR Images (Processed)

- Download `RDRCUMINDEX.tab` from: https://hirise-pds.lpl.arizona.edu/PDS/INDEX/
- Update the `cumindex_loc` path in `code/download_hirise_rdr.s` (line 26)

### 2. Make Scripts Executable

```bash
chmod +x code/download_ctx.s
chmod +x code/download_hirise_edr.s
chmod +x code/download_hirise_rdr.s
```

## Usage

### Individual Image Downloads (Shell Scripts)

You can download single images using the shell scripts directly:

#### CTX Images

```bash
./code/download_ctx.s B05_011631_2208_XI_40N311W
```

- Downloads a CTX .IMG file
- Creates files in the current directory

#### HiRISE EDR Images (Raw Data)

```bash
./code/download_hirise_edr.s ESP_052535_2225
```

- Downloads raw HiRISE .IMG files (RED0-9, BG0-1, IR0-1)
- Creates a subdirectory named after the image ID
- Downloads all associated .IMG files for that observation

#### HiRISE RDR Images (Processed Data)

```bash
./code/download_hirise_rdr.s ESP_052535_2225
```

- Downloads processed HiRISE .JP2 files
- Downloads all associated .JP2 files (RED, COLOR) for that observation

### Batch Downloads (Multiple Downloader)

For downloading multiple images efficiently with parallel processing:

#### 1. Compile the Program

```bash
make
```

This creates an executable named `run`.

#### 2. Create Image List File

Create a text file with one image ID per line:

**ctx_images.txt:**

```
B05_011631_2208_XI_40N311W
B16_016076_2177_XI_37N344W
B05_011697_2212_XI_41N312W
G15_024238_2255_XI_45N338W
P01_001414_2164_XI_36N008W
U19_078358_2172_XI_37N337W
```

**hirise_images.txt:**

```
ESP_018427_2640
ESP_052324_2225
ESP_052535_2225
ESP_088397_1710
ESP_088395_1680
ESP_088394_2650
```

#### 3. Run the Multiple Downloader

**Basic usage (downloads to current directory):**

```bash
./run ctx ctx_images.txt
./run hirise_edr hirise_images.txt
./run hirise_rdr hirise_images.txt
```

**Download to specific directory:**

```bash
./run ctx ctx_images.txt ./ctx_images
./run hirise_edr hirise_images.txt hirise_edr_images
./run hirise_rdr hirise_images.txt /home/user/hirise_rdr_images
```

#### Command Line Arguments

- `<image_type>`: Type of images to download
  - `ctx` - CTX images (uses `download_ctx.s`)
  - `hirise_edr` - HiRISE raw images (uses `download_hirise_edr.s`)
  - `hirise_rdr` - HiRISE processed images (uses `download_hirise_rdr.s`)
- `<image_list>` - Text file containing image IDs (one per line)
- `[directory]` - Optional download directory (defaults to current directory)

## Features

### Multiple Downloader Features

- **Parallel Processing**: Downloads 5 images simultaneously by default
  - Change NUM_PROCESS based on # of processors and/or network connection
- **Sliding Window**: Maintains constant parallel downloads - starts new downloads immediately as others complete
- **Progress Tracking**: Shows real-time progress with PID tracking
- **Flexible Paths**: Works with relative or absolute paths

## Configuration

### Adjusting Parallel Downloads

Edit `NUM_PROCESS` in `multiple_downloader.cpp` to change the number of simultaneous downloads:

```cpp
#define NUM_PROCESS 5  // Change this value
```

Then recompile:

```bash
make clean
make
```

### Script Paths

The multiple downloader expects scripts to be in the `code/` subdirectory. If your scripts are elsewhere, update the paths in `multiple_downloader.cpp`:

```cpp
download_script = curr_dir + "/your_path/download_ctx.s";
```

## Troubleshooting

### Common Issues

1. **Script not found error**
   
   - Ensure scripts are in the `code/` directory
   - Check that scripts are executable (`chmod +x`)
   - Verify cumulative index file paths in scripts

2. **No images downloaded**
   
   - Check that cumulative index files are up to date
   - Verify image IDs exist in the index files
   - Ensure internet connection is stable

3. **Permission errors**
   
   - Check write permissions for download directory
   - Ensure scripts have execute permissions

## Cleanup

Remove the compiled executable:

```bash
make clean
```

## Notes

- Ensure sufficient disk space before starting large batch downloads
- The multiple downloader uses absolute paths, so it works regardless of execution directory

## Credits

Shell scripts originally written by Euibin Kim  
Sliding window implementation added July 2025