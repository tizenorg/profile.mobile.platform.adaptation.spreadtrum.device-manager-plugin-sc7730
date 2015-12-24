Name:       device-manager-plugin-sc7730
Summary:    Device manager plugin sc7730
Version: 0.0.1
Release:    0
Group:      System/Hardware Adaptation
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1:    %{name}.manifest
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(hwcommon)
BuildRequires:  pkgconfig(glib-2.0)

%description
Device manager plugin sc7730


%prep
%setup -q
cp %{SOURCE1} .

%build
%cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}

mkdir -p %{buildroot}%{_prefix}/lib/udev/rules.d
install -m 644 61-tizen-video-device.rules %{buildroot}%{_prefix}/lib/udev/rules.d

%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%{_libdir}/hw/*.so
%manifest %{name}.manifest
%license LICENSE
%{_prefix}/lib/udev/rules.d/61-tizen-video-device.rules
