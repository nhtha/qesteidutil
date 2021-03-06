﻿<?xml version="1.0" encoding="utf-8"?>
<Package xmlns="http://schemas.microsoft.com/appx/manifest/foundation/windows10"
    xmlns:uap="http://schemas.microsoft.com/appx/manifest/uap/windows10"
    xmlns:rescap="http://schemas.microsoft.com/appx/manifest/foundation/windows10/restrictedcapabilities">
  <Identity Name="RiigiInfossteemiAmet.ID-cardutility" ProcessorArchitecture="${PLATFORM}" Version="${PROJECT_VERSION}.0"
    Publisher="CN=8BBBE4D8-620A-4884-A12A-72F1A2030D8B" />
  <Properties>
    <DisplayName>ID-card utility</DisplayName>
    <PublisherDisplayName>Riigi Infosüsteemi Amet</PublisherDisplayName>
    <Logo>Assets\Utility.50x50.png</Logo>
  </Properties>
  <Resources>
    <Resource Language="en-us" />
  </Resources>
  <Dependencies>
    <TargetDeviceFamily Name="Windows.Desktop" MinVersion="10.0.14393.0" MaxVersionTested="10.0.15063.0" />
    <PackageDependency Name="Microsoft.VCLibs.140.00.UWPDesktop" MinVersion="14.0.24217.0"
      Publisher="CN=Microsoft Corporation, O=Microsoft Corporation, L=Redmond, S=Washington, C=US" />
  </Dependencies>
  <Capabilities>
    <rescap:Capability Name="runFullTrust" />
  </Capabilities>
  <Applications>
    <Application Id="Utility" Executable="qesteidutil.exe" EntryPoint="Windows.FullTrustApplication">
      <uap:VisualElements DisplayName="ID-card utility" Description="ID-card utility" BackgroundColor="#00355f"
        Square150x150Logo="Assets\Utility.150x150.png" Square44x44Logo="Assets\Utility.44x44.png" />
    </Application>
  </Applications>
</Package>
