import os
import SimpleITK as sitk

def convert_dicom_to_nifti_manual(dicom_dir, output_filename="output.nii", flip_y=True):
    # Collect all DICOM files
    dicom_files = [os.path.join(dicom_dir, f) for f in os.listdir(dicom_dir) if f.endswith(".dcm")]
    dicom_files.sort()

    if not dicom_files:
        raise ValueError(f"No DICOM files found in {dicom_dir}")

    # Read DICOM image
    reader = sitk.ImageSeriesReader()
    reader.SetFileNames(dicom_files)
    image = reader.Execute()

    # Flip Y axis if needed
    if flip_y:
        image = sitk.Flip(image, [False, True, False])  # Flip only the Y-axis

    # Save to NIfTI
    sitk.WriteImage(image, output_filename)
    print(f"Saved NIfTI to: {output_filename}")

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Convert a folder of DICOM files to NIfTI (.nii) with optional Y-flip.")
    parser.add_argument("dicom_dir", help="Path to the folder with .dcm files")
    parser.add_argument("--output", default="output.nii", help="Output filename")
    parser.add_argument("--no-flip", action="store_true", help="Disable Y-axis flipping")

    args = parser.parse_args()
    convert_dicom_to_nifti_manual(args.dicom_dir, args.output, flip_y=not args.no_flip)