param(
    [string]$VSPath,
    [string]$Config = "Release",
    [string]$Platform = "x64"
)

if ($Platform -ne "x64") {
    Write-Error "This build script supports x64 only."
    exit 1
}

# VS のインストール先に VsDevCmd.bat がない場合、MSBuild を直接探す
if (-not $VSPath) {
    $VSPath = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -property installationPath
    Write-Host "Found VS at: $VSPath"
}

# MSBuild のパスを構築
$MSBuildPath = Join-Path $VSPath "MSBuild\Current\Bin\MSBuild.exe"

if (-not (Test-Path $MSBuildPath)) {
    Write-Error "MSBuild not found at $MSBuildPath"
    exit 1
}

Write-Host "MSBuild found at: $MSBuildPath"

# Step 1: shared
Write-Host "`n=== Step 1: Building shared.dll ===" -ForegroundColor Green
& $MSBuildPath "shared\shared.vcxproj" "/p:Configuration=$Config" "/p:Platform=$Platform" "/nologo"

if ($LASTEXITCODE -ne 0) {
    Write-Error "shared build failed"
    exit 1
}

$SharedDll = Join-Path "shared\x64\$Config" "shared.dll"
if (-not (Test-Path $SharedDll)) {
    Write-Error "shared.dll not found at: $SharedDll"
    exit 1
}

# Step 2: foo_favorite
Write-Host "`n=== Step 2: Building foo_favorite.dll ===" -ForegroundColor Green
& $MSBuildPath "foobar2000\foo_favorite\foo_favorite.vcxproj" "/p:Configuration=$Config" "/p:Platform=$Platform" "/nologo"

if ($LASTEXITCODE -ne 0) {
    Write-Error "foo_favorite build failed"
    exit 1
}

$OutDll = Join-Path "foobar2000\foo_favorite\x64\$Config" "foo_favorite.dll"
if (-not (Test-Path $OutDll)) {
    Write-Error "DLL not found at: $OutDll"
    exit 1
}

# Step 3: Package component
Write-Host "`n=== Step 3: Packaging foo_favorite.fb2k-component ===" -ForegroundColor Green
$PackageRoot = "foobar2000\foo_favorite\_package"
$ResultDir = "foobar2000\foo_favorite\_result"
$PackageFile = Join-Path $ResultDir "foo_favorite.fb2k-component"

if (Test-Path $PackageRoot) {
    Remove-Item $PackageRoot -Recurse -Force
}
New-Item -ItemType Directory -Path $PackageRoot -Force | Out-Null

Copy-Item $OutDll -Destination (Join-Path $PackageRoot "foo_favorite.dll") -Force
Copy-Item $SharedDll -Destination (Join-Path $PackageRoot "shared.dll") -Force

if (-not (Test-Path $ResultDir)) {
    New-Item -ItemType Directory -Path $ResultDir -Force | Out-Null
}

if (Test-Path $PackageFile) {
    Remove-Item $PackageFile -Force
}

$ZipTemp = [System.IO.Path]::ChangeExtension($PackageFile, ".zip")
if (Test-Path $ZipTemp) {
    Remove-Item $ZipTemp -Force
}

Compress-Archive -Path (Join-Path $PackageRoot "*") -DestinationPath $ZipTemp -Force -ErrorAction Stop
Move-Item -Path $ZipTemp -Destination $PackageFile -Force

if (-not (Test-Path $PackageFile)) {
    Write-Error "Component package not created: $PackageFile"
    exit 1
}

Write-Host "`n=== Build Complete ===" -ForegroundColor Green
Write-Host "DLL: $OutDll"
Write-Host "Shared DLL: $SharedDll"
Write-Host "Component: $PackageFile"
