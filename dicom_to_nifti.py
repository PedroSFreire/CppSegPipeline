import os
import SimpleITK as sitk

def convert_dicom_to_nifti_for_slicer(dicom_dir, output_filename="output.nii"):
    # Collect DICOM files
    dicom_files = [os.path.join(dicom_dir, f) for f in os.listdir(dicom_dir) if f.endswith(".dcm")]
    dicom_files.sort()

    if not dicom_files:
        raise ValueError(f"No DICOM files found in {dicom_dir}")

    # Read DICOM series
    reader = sitk.ImageSeriesReader()
    reader.SetFileNames(dicom_files)
    image = reader.Execute()

    # Fix orientation: adjust direction matrix to standard RAS (used in NIfTI)
    direction = list(image.GetDirection())
    # Flip Y-axis direction (posterior -> anterior)
    direction[4] *= -1
    image.SetDirection(direction)

    # Save as NIfTI
    sitk.WriteImage(image, output_filename)
    print(f"Saved corrected NIfTI to: {output_filename}")

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Convert DICOM to NIfTI for 3D Slicer compatibility.")
    parser.add_argument("dicom_dir", help="Folder with .dcm files")
    parser.add_argument("--output", default="output.nii", help="Output NIfTI file name")

    args = parser.parse_args()
    convert_dicom_to_nifti_for_slicer(args.dicom_dir, args.output)