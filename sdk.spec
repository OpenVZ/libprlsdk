Summary: Software Developing Kit for Parallels Cloud Server
Name: parallels-sdk 
Version: 7.0.0
Release: 1%{?dist}

Group: Development/Libraries	
License: LGPLv2+
URL: https://github.com/Parallels/parallels-sdk
Source: parallels-sdk-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

BuildRequires: make
BuildRequires: gcc
BuildRequires: gcc-c++
BuildRequires: python
BuildRequires: python-devel
BuildRequires: qt4-devel
BuildRequires: qt4


%description
Parallels Virtualization SDK is a development kit used to create and integrate
custom software solutions with Parallels Cloud Server. The SDK provides cross-
platform ANSI C and Python APIs.

%prep
%setup -q


%build
export PATH="$PATH:/usr/lib64/qt4/bin"
cd Sources
./Gen.py release
cd SDK
qmake-qt4
make %{?_smp_mflags} release


%install
cd Sources/SDK
make install
rm -rf %{buildroot}

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)

%dir %{_bindir}

%doc

%changelog
* Wed Sep 17 2014 Maxim Nestratov <mnestratov@parallels.com> - 7.0.0
- Initial public revision
