#sbs-git:slp/pkgs/d/device-manager-plugin-sc7727 device-manager-plugin-sc7727 0.0.1 5bf2e95e0bb15c43ff928f7375e1978b0accb0f8
Name:       device-manager-plugin-sc7730
Summary:    Device manager plugin sc7730
Version: 0.0.03
Release:    0
Group:      TO_BE/FILLED_IN
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig(devman_plugin)
BuildRequires:  pkgconfig(dlog)

%if "%{tizen_target_name}" != "Z300H"
ExcludeArch: %{arm}
%endif

%description
Device manager plugin sc7730


%prep
%setup -q

%build
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
cp -a apache-2.0.txt %{buildroot}/usr/share/license/%{name}
mkdir -p %{buildroot}/usr/lib/udev/rules.d
cp -a 61-tizen-video-device.rules %{buildroot}/usr/lib/udev/rules.d

%make_install

%post
/sbin/ldconfig
mkdir -p /lib/firmware/modem

%postun
/sbin/ldconfig

%files
%manifest device-manager-plugin-sc7727.manifest
/usr/share/license/device-manager-plugin-sc7730
/usr/lib/libslp_devman_plugin.so
/usr/lib/udev/rules.d/61-tizen-video-device.rules
/opt/etc/dump.d/module.d/
%exclude /usr/bin/sys_oal_test
