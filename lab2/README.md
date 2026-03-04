# Lab 2 - FIR Filters

### Objective

Design and implement FIR filters using sample-by-sample processing.

### Description

Develop an program capable of applying a sample-based FIR filter to a 512-sample memory buffer, acquired in Lab #1, considering sampling frequencies such as 8 kHz, 16 kHz, 22 kHz, 44 kHz, and 48 kHz. Using one of the buttons on the MCXN947 evaluation board, the filter application will be enabled or a bypass will be performed, sending the data to an output buffer different from the input buffer. From this output buffer, the samples will be sent to the DAC of the MCXN947 board. Visualize the result using an oscilloscope and compare the outputs.

#### Filter Requirements:

- Low-Pass: Fc = 3600 Hz, Astop = 30 dB.
- High-Pass: Fc = 35 Hz, Astop = 30 dB.
- Band-Pass: Fc1 = 35 Hz, Fc2 = 3500 Hz, Astop = 30 dB.
- Band-Stop: Fr = 50 Hz, Bw = 15 Hz, Astop = 25 dB.

### Presentation

Upload a file containing (one per group, include all members' names):

1. Show the program running.
2. Application design diagram.
3. Source code with the complete project in KDS.
4. Photos of the most significant waveforms.
