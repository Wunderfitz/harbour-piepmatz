Name:       harbour-piepmatz

%{!?qtc_qmake:%define qtc_qmake %qmake}
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}
Summary:    Piepmatz is a Twitter client for Sailfish OS
Version:    1.13
Release:    2
License:    GPLv3
URL:        http://werkwolf.de/
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Positioning)
BuildRequires:  pkgconfig(qt5embedwidget)
BuildRequires:  pkgconfig(libsailfishkeyprovider)
BuildRequires:  desktop-file-utils

%description
Piepmatz is a Twitter client for Sailfish OS


%prep
%setup -q -n %{name}-%{version}

%build

%qtc_qmake5 \
          %{?piepmatz_twitter_client_id: TWITTER_CLIENT_ID=%{piepmatz_twitter_client_id}} \
          %{?piepmatz_twitter_client_secret: TWITTER_CLIENT_SECRET=%{piepmatz_twitter_client_secret}}


%qtc_make %{?_smp_mflags}

%install
%qmake5_install

desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications             \
   %{buildroot}%{_datadir}/applications/*.desktop

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
