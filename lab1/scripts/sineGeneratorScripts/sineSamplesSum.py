import numpy as np
import matplotlib.pyplot as plt

# Parameters
num_samples = 512        # Number of points
amplitude1 = 2047        # Amplitude of first signal (half-scale)
amplitude2 = 1023        # Amplitude of second signal
freq1 = 10000             # Frequency of first sine wave
freq2 = 500              # Frequency of second sine wave
sample_rate = 512        # Samples per second (Nyquist base for demonstration)

# Generate sample points (0 to num_samples-1)
t = np.arange(num_samples)

# --- Generate two sine waves ---
wave1 = amplitude1 * np.sin(2 * np.pi * freq1 * t / sample_rate)
wave2 = amplitude2 * np.sin(2 * np.pi * freq2 * t / sample_rate)

# --- Sum both signals and offset to positive range (0–4095 for 12-bit DAC) ---
combined = wave1 + wave2
combined_shifted = (combined - np.min(combined)) * (4095 / (np.max(combined) - np.min(combined)))

# Convert to uint16 for DAC
wave_int = np.round(combined_shifted).astype(np.uint16)

# --- Save to C array format ---
output_filename = "mixed_wave_512.c"

with open(output_filename, "w") as f:
    f.write(f"// {num_samples}-sample mixed sine wave for 12-bit DAC\n")
    f.write(f"uint16_t waveForm[{num_samples}] = {{\n")

    # Write 8 values per line for readability
    for i in range(0, num_samples, 8):
        line = ", ".join(f"{v:5d}" for v in wave_int[i:i+8])
        if i + 8 < num_samples:
            f.write(f"    {line},\n")
        else:
            f.write(f"    {line}\n")

    f.write("};\n")

print(f"\n C array saved to '{output_filename}' successfully!")

# --- Plot ---
plt.figure(figsize=(10,4))
plt.plot(t, wave_int, label="Mixed Wave (freq1 + freq2)")
plt.title("Mixed Sine Waves (12-bit DAC Range 0–4095)")
plt.xlabel("Sample Index")
plt.ylabel("Amplitude")
plt.grid(True)
plt.legend()
plt.show()

