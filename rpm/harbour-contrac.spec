Name:       harbour-contrac

%define version_major 0
%define version_minor 7
%define version_revis 10
%if %{defined sailfishos_version}
%define sailfish_version %{sailfishos_version}
%else
%define sailfish_version %( if [ -f /bin/awk ]; then awk -F= '$1=="VERSION_ID" { print $2 ;}' /etc/os-release | cut -d'.' -f1-3 | sed 's/\\./0/g'; fi )
%endif

Summary:    Contrac
Version:    %{version_major}.%{version_minor}.%{version_revis}
Release:    1
Group:      Qt/Qt
License:    GPLv2
URL:        https://www.flypig.co.uk/contrac
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   openssl
Requires:   protobuf-lite
Requires:   sailfishsecretsdaemon-secretsplugins-default
Requires:   qr-filter-qml-plugin
BuildRequires:  make
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(openssl)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(protobuf-lite)
BuildRequires:  pkgconfig(libcurl)
BuildRequires:  pkgconfig(libxml-2.0)
%if 0%{?sailfish_version} < 40500
BuildRequires:  pkgconfig(quazip)
%else
BuildRequires:  pkgconfig(quazip1-qt5)
%endif
BuildRequires:  pkgconfig(keepalive)
BuildRequires:  pkgconfig(nemonotifications-qt5)
BuildRequires:  pkgconfig(sailfishsecrets)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  protobuf-compiler
BuildRequires:  qt5-qttools-linguist
BuildRequires:  desktop-file-utils

%description
Exposure Notification app compatible with
Germany's Corona Warn App

%if "%{?vendor}" == "chum"
PackageName: contrac
Type: desktop-application
Categories:
  - System
Custom:
  Repo: https://github.com/llewelld/harbour-contrac
Icon: https://github.com/llewelld/harbour-contrac/raw/master/icons/256x256/harbour-contrac.png
%endif

%package tests
Summary:    Tests for %{name}
Group:      Qt/Qt
Requires:   %{name} = %{version}-%{release}
Requires:   qt5-qtdeclarative-import-qttest
BuildRequires:  pkgconfig(Qt5QuickTest)

%description tests
Unit tests for %{name}

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5 DEFINES+='VERSION_MAJOR=%{version_major}' \
  DEFINES+='VERSION_MINOR=%{version_minor}' \
  DEFINES+='VERSION_REVIS=%{version_revis}' \
  DEFINES+='VERSION=\"\\\"\"%{version_major}.%{version_minor}.%{version_revis}\"\\\"\"' \
  SAILFISH_VERSION='%{sailfish_version}' \
  %{name}.pro
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install
desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications             \
   %{buildroot}%{_datadir}/applications/*.desktop

%post
if [ "$1" -ge 1 ]; then
systemctl-user daemon-reload || true
systemctl-user restart contracd.service || true
systemctl-user enable contracd.service || true
fi

%postun
if [ "$1" -eq 0 ]; then
systemctl-user stop contracd.service || true
systemctl-user disable contracd.service || true
systemctl-user daemon-reload || true
fi

%files
%defattr(-,root,root,-)
%{_bindir}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/%{name}/translations
%{_bindir}/contracd
%{_userunitdir}/contracd.service
%{_datadir}/mapplauncherd/privileges.d/contracd.privileges

%files tests
%defattr(-,root,root,-)
/opt/tests/%{name}-tests
%{_datadir}/%{name}-tests/sample_diagnosis_key_file.zip
