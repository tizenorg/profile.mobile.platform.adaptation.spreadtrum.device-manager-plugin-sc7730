#--------------------------------------
#   		power
#--------------------------------------
PM_DEBUG=$1/power
PM_DEBUG_PATH_HISTORY=sleep_history.log
PM_DEBUG_PATH_WAKEUP_SOURCE=wakeup_source.log
PM_DEBUG_PATH_WAKELOCK=slave_wakelocks.log
PM_DEBUG_PATH_RUNTIME=runtime_pm_activetime.log
PM_DEBUG_PATH_SLP_MON_INFO=slp_mon_info.log

PM_KMG_HISTORY=/sys/kernel/debug/sleep_history
PM_KMG_WAKEUP_SOURCE=/sys/kernel/debug/wakeup_sources
PM_KMG_WAKELOCK=/sys/kernel/debug/slave_wakelocks
PM_KMG_RUNTIME=/sys/class/runtime_pm/test/active_time
PM_KMG_SLP_MON_INFO=/sys/kernel/debug/sleep_monitor/dev_name

/bin/mkdir -p ${PM_DEBUG}

if [ -e ${PM_KMG_HISTORY} ];
then
/bin/cat ${PM_KMG_HISTORY} > ${PM_DEBUG}/${PM_DEBUG_PATH_HISTORY}
fi

if [ -e ${PM_KMG_WAKEUP_SOURCE} ];
then
/bin/cat ${PM_KMG_WAKEUP_SOURCE} > ${PM_DEBUG}/${PM_DEBUG_PATH_WAKEUP_SOURCE}
fi

if [ -e ${PM_KMG_WAKELOCK} ];
then
/bin/cat ${PM_KMG_WAKELOCK} > ${PM_DEBUG}/${PM_DEBUG_PATH_WAKELOCK}
fi

if [ -e ${PM_KMG_RUNTIME} ];
then
/bin/cat ${PM_KMG_RUNTIME} > ${PM_DEBUG}/${PM_DEBUG_PATH_RUNTIME}
fi

if [ -e ${PM_KMG_SLP_MON_INFO} ];
then
/bin/cat ${PM_KMG_SLP_MON_INFO} > ${PM_DEBUG}/${PM_DEBUG_PATH_SLP_MON_INFO}
fi
