import React, { useState, useEffect, useRef } from 'react';
import { Clock, DollarSign, Calendar, Coffee, Play, Pause, AlertCircle, User, UserPlus, Trash2, Users } from 'lucide-react';

export default function TimeCardApp() {
  const [profiles, setProfiles] = useState([]);
  const [currentProfile, setCurrentProfile] = useState(null);
  const [showProfileManager, setShowProfileManager] = useState(false);
  const [newProfileName, setNewProfileName] = useState('');
  const [showNewProfileForm, setShowNewProfileForm] = useState(false);
  const [showDeleteConfirm, setShowDeleteConfirm] = useState(false);
  const [profileToDelete, setProfileToDelete] = useState(null);
  const [language, setLanguage] = useState('en');
  
  const [hourlyRate, setHourlyRate] = useState(15);
  const [payPeriod, setPayPeriod] = useState('weekly');
  const [isClockedIn, setIsClockedIn] = useState(false);
  const [clockInTime, setClockInTime] = useState(null);
  const [workSessions, setWorkSessions] = useState([]);
  const [currentTime, setCurrentTime] = useState(new Date());
  const [showSettings, setShowSettings] = useState(false);
  const [breakTimer, setBreakTimer] = useState(null);
  const [breakTimeLeft, setBreakTimeLeft] = useState(0);
  const [isOnBreak, setIsOnBreak] = useState(false);
  const breakIntervalRef = useRef(null);

  // Translations
  const translations = {
    en: {
      systemHeader: "═══ EMPLOYEE TIME CARD SYSTEM ═══",
      appTitle: "TIMECARD",
      terminal: "CLOCK IN/OUT TERMINAL",
      selectProfile: "┌─ SELECT EMPLOYEE PROFILE ─┐",
      noProfiles: "[ NO EMPLOYEE PROFILES FOUND ]",
      addEmployee: "▶ ADD NEW EMPLOYEE",
      employeeName: "EMPLOYEE NAME:",
      enterName: "ENTER FULL NAME",
      create: "CREATE",
      cancel: "CANCEL",
      delete: "DELETE",
      profileManagement: "═══ PROFILE MANAGEMENT ═══",
      active: "[ACTIVE]",
      select: "SELECT",
      del: "DEL",
      empSince: "EMP. SINCE:",
      deleteProfile: "DELETE PROFILE?",
      employee: "EMPLOYEE:",
      dataDeleted: "ALL DATA WILL BE PERMANENTLY DELETED!",
      areYouSure: "ARE YOU SURE?",
      yes: "YES",
      no: "NO",
      profiles: "PROFILES",
      currentTime: "══════ CURRENT TIME ══════",
      notClockedIn: "STATUS: NOT CLOCKED IN",
      offDuty: "▬▬ OFF DUTY ▬▬",
      punchIn: "▶▶▶ PUNCH IN ◀◀◀",
      onDuty: "═══ STATUS: ON DUTY ═══",
      clockedInAt: "CLOCKED IN AT:",
      hours: "HOURS:",
      selectBreak: "┌─ SELECT BREAK DURATION ─┐",
      min15: "15 MIN",
      min30: "30 MIN",
      hour1: "1 HOUR",
      onBreak: "═══ ON BREAK ═══",
      endBreak: "END BREAK",
      punchOut: "▶▶▶ PUNCH OUT ◀◀◀",
      earnings: "═══ EARNINGS SUMMARY ═══",
      todayEarnings: "TODAY'S TOTAL EARNINGS:",
      hoursWorked: "HOURS WORKED:",
      hourlyRate: "HOURLY RATE:",
      paySchedule: "PAY SCHEDULE:",
      configSettings: "▼ CONFIGURATION SETTINGS",
      hourlyRateLabel: "HOURLY RATE ($):",
      payPeriodLabel: "PAY PERIOD SCHEDULE:",
      language: "LANGUAGE:",
      weekly: "WEEKLY",
      biWeekly: "BI-WEEKLY",
      monthly: "MONTHLY",
      biMonthly: "BI-MONTHLY (15TH & EOM)",
      yearly: "YEARLY",
      workHistory: "═══ WORK HISTORY ═══",
      clear: "CLEAR",
      date: "DATE:",
      in: "IN:",
      out: "OUT:",
      hrs: "HRS",
      weeklyNext: "Weekly (Next:",
      biWeeklyNext: "Bi-Weekly (Next:",
      monthlyNext: "Monthly (Next:",
      biMonthlyNext: "Bi-Monthly (Next:",
      yearlyNext: "Yearly (Next:"
    },
    es: {
      systemHeader: "═══ SISTEMA DE TARJETA DE TIEMPO ═══",
      appTitle: "TARJETA DE TIEMPO",
      terminal: "TERMINAL DE ENTRADA/SALIDA",
      selectProfile: "┌─ SELECCIONAR PERFIL DE EMPLEADO ─┐",
      noProfiles: "[ NO SE ENCONTRARON PERFILES ]",
      addEmployee: "▶ AGREGAR NUEVO EMPLEADO",
      employeeName: "NOMBRE DEL EMPLEADO:",
      enterName: "INGRESE NOMBRE COMPLETO",
      create: "CREAR",
      cancel: "CANCELAR",
      delete: "ELIMINAR",
      profileManagement: "═══ GESTIÓN DE PERFILES ═══",
      active: "[ACTIVO]",
      select: "SELECCIONAR",
      del: "ELIM",
      empSince: "EMP. DESDE:",
      deleteProfile: "¿ELIMINAR PERFIL?",
      employee: "EMPLEADO:",
      dataDeleted: "¡TODOS LOS DATOS SERÁN ELIMINADOS PERMANENTEMENTE!",
      areYouSure: "¿ESTÁ SEGURO?",
      yes: "SÍ",
      no: "NO",
      profiles: "PERFILES",
      currentTime: "══════ HORA ACTUAL ══════",
      notClockedIn: "ESTADO: NO REGISTRADO",
      offDuty: "▬▬ FUERA DE SERVICIO ▬▬",
      punchIn: "▶▶▶ ENTRADA ◀◀◀",
      onDuty: "═══ ESTADO: EN SERVICIO ═══",
      clockedInAt: "REGISTRADO A LAS:",
      hours: "HORAS:",
      selectBreak: "┌─ SELECCIONAR DURACIÓN DEL DESCANSO ─┐",
      min15: "15 MIN",
      min30: "30 MIN",
      hour1: "1 HORA",
      onBreak: "═══ EN DESCANSO ═══",
      endBreak: "TERMINAR DESCANSO",
      punchOut: "▶▶▶ SALIDA ◀◀◀",
      earnings: "═══ RESUMEN DE GANANCIAS ═══",
      todayEarnings: "GANANCIAS TOTALES DE HOY:",
      hoursWorked: "HORAS TRABAJADAS:",
      hourlyRate: "TARIFA POR HORA:",
      paySchedule: "CALENDARIO DE PAGO:",
      configSettings: "▼ CONFIGURACIÓN",
      hourlyRateLabel: "TARIFA POR HORA ($):",
      payPeriodLabel: "PERÍODO DE PAGO:",
      language: "IDIOMA:",
      weekly: "SEMANAL",
      biWeekly: "QUINCENAL",
      monthly: "MENSUAL",
      biMonthly: "BIMENSUAL (15 Y FIN DE MES)",
      yearly: "ANUAL",
      workHistory: "═══ HISTORIAL DE TRABAJO ═══",
      clear: "BORRAR",
      date: "FECHA:",
      in: "ENTRADA:",
      out: "SALIDA:",
      hrs: "HRS",
      weeklyNext: "Semanal (Próximo:",
      biWeeklyNext: "Quincenal (Próximo:",
      monthlyNext: "Mensual (Próximo:",
      biMonthlyNext: "Bimensual (Próximo:",
      yearlyNext: "Anual (Próximo:"
    },
    ht: {
      systemHeader: "═══ SISTÈM KAT TAN ANPLWAYE ═══",
      appTitle: "KAT TAN",
      terminal: "TÈMINAL ANTRE/SOTI",
      selectProfile: "┌─ CHWAZI PWOFIL ANPLWAYE ─┐",
      noProfiles: "[ PA GEN PWOFIL JWENN ]",
      addEmployee: "▶ AJOUTE NOUVO ANPLWAYE",
      employeeName: "NON ANPLWAYE:",
      enterName: "ANTRE NON KONPLÈ",
      create: "KREYE",
      cancel: "ANILE",
      delete: "EFASE",
      profileManagement: "═══ JESYON PWOFIL ═══",
      active: "[AKTIF]",
      select: "CHWAZI",
      del: "EFAS",
      empSince: "ANPL. DEPI:",
      deleteProfile: "EFASE PWOFIL?",
      employee: "ANPLWAYE:",
      dataDeleted: "TOUT DONE YO AP EFASE PÈMANAN!",
      areYouSure: "OU ASIRE?",
      yes: "WI",
      no: "NON",
      profiles: "PWOFIL",
      currentTime: "══════ LÈ KOUNYE A ══════",
      notClockedIn: "ESTATI: PA ANREJISTRE",
      offDuty: "▬▬ PA SOU TRAVAY ▬▬",
      punchIn: "▶▶▶ ANTRE ◀◀◀",
      onDuty: "═══ ESTATI: SOU TRAVAY ═══",
      clockedInAt: "ANREJISTRE A:",
      hours: "LÈ:",
      selectBreak: "┌─ CHWAZI DIRE REPO ─┐",
      min15: "15 MIN",
      min30: "30 MIN",
      hour1: "1 LÈ",
      onBreak: "═══ SOU REPO ═══",
      endBreak: "FINI REPO",
      punchOut: "▶▶▶ SOTI ◀◀◀",
      earnings: "═══ REZIME PEMAN ═══",
      todayEarnings: "TOTAL PEMAN JOU A:",
      hoursWorked: "LÈ TRAVAY:",
      hourlyRate: "TO PA LÈ:",
      paySchedule: "ORÈ PEMAN:",
      configSettings: "▼ KONFIGIRASYON",
      hourlyRateLabel: "TO PA LÈ ($):",
      payPeriodLabel: "PERYÒD PEMAN:",
      language: "LANG:",
      weekly: "CHAK SEMÈN",
      biWeekly: "CHAK DE SEMÈN",
      monthly: "CHAK MWA",
      biMonthly: "DE FWA PA MWA (15 AK FEN MWA)",
      yearly: "CHAK ANE",
      workHistory: "═══ ISTWA TRAVAY ═══",
      clear: "EFASE",
      date: "DAT:",
      in: "ANTRE:",
      out: "SOTI:",
      hrs: "LÈ",
      weeklyNext: "Chak Semèn (Pwochen:",
      biWeeklyNext: "Chak De Semèn (Pwochen:",
      monthlyNext: "Chak Mwa (Pwochen:",
      biMonthlyNext: "De Fwa Pa Mwa (Pwochen:",
      yearlyNext: "Chak Ane (Pwochen:"
    },
    lc: {
      systemHeader: "═══ SISTÈM KAT D'TAN TRAVAYÈ ═══",
      appTitle: "KAT D'TAN",
      terminal: "TÈRMINAL RANTRE/SORTI",
      selectProfile: "┌─ CHWÉZI PROFIL TRAVAYÈ ─┐",
      noProfiles: "[ PA GEN PROFIL TROUVÉ ]",
      addEmployee: "▶ AJOUTE NOUVO TRAVAYÈ",
      employeeName: "NOM TRAVAYÈ:",
      enterName: "MÉTÉ NOM KONPLÈ",
      create: "KRÉYÉ",
      cancel: "ANILÉ",
      delete: "JÉTÉ",
      profileManagement: "═══ JESTION DE PROFIL ═══",
      active: "[AKTIF]",
      select: "CHWÉZI",
      del: "JÉTÉ",
      empSince: "TRAVAYÈ DÈPI:",
      deleteProfile: "JÉTÉ PROFIL?",
      employee: "TRAVAYÈ:",
      dataDeleted: "TOUT BAGAY POU DISPARET POU TOUJOU!",
      areYouSure: "TO SÛR?",
      yes: "WI",
      no: "NON",
      profiles: "PROFIL",
      currentTime: "══════ LHEUR ASTEUR ══════",
      notClockedIn: "ÉSTA: PA RANZISTRÉ",
      offDuty: "▬▬ PA SOU TRAVAY ▬▬",
      punchIn: "▶▶▶ RANTRE ◀◀◀",
      onDuty: "═══ ÉSTA: SOU TRAVAY ═══",
      clockedInAt: "RANZISTRÉ A:",
      hours: "LHEUR:",
      selectBreak: "┌─ CHWÉZI KONBYIN TAN RÈPO ─┐",
      min15: "15 MIN",
      min30: "30 MIN",
      hour1: "1 LHEUR",
      onBreak: "═══ SOU RÈPO ═══",
      endBreak: "FINI RÈPO",
      punchOut: "▶▶▶ SORTI ◀◀◀",
      earnings: "═══ RÉZYUMÉ PÈYMAN ═══",
      todayEarnings: "TOTAL PÈYMAN JODI:",
      hoursWorked: "LHEUR TRAVAYÉ:",
      hourlyRate: "TO PAR LHEUR:",
      paySchedule: "KALINDYÉ PÈYMAN:",
      configSettings: "▼ KONFIGIRASYON",
      hourlyRateLabel: "TO PAR LHEUR ($):",
      payPeriodLabel: "PÉRYOD PÈYMAN:",
      language: "LANG:",
      weekly: "CHAK SEMENN",
      biWeekly: "CHAK DÉ SEMENN",
      monthly: "CHAK MWA",
      biMonthly: "DÉ FWA PAR MWA (15 É FIN MWA)",
      yearly: "CHAK LANNÉ",
      workHistory: "═══ LISTWAR TRAVAY ═══",
      clear: "JÉTÉ TOU",
      date: "DAT:",
      in: "RANTRE:",
      out: "SORTI:",
      hrs: "LHEUR",
      weeklyNext: "Chak Semenn (Prochain:",
      biWeeklyNext: "Chak Dé Semenn (Prochain:",
      monthlyNext: "Chak Mwa (Prochain:",
      biMonthlyNext: "Dé Fwa par Mwa (Prochain:",
      yearlyNext: "Chak Lanné (Prochain:"
    },
    pd: {
      systemHeader: "═══ ZEITKADDEL SISCHDEM ═══",
      appTitle: "ZEITKADDEL",
      terminal: "EI-UND-AUS TIRMINAL",
      selectProfile: "┌─ GUCK EN WADDA PROFIL ─┐",
      noProfiles: "[ KEE WADDA PROFILE GFUNNE ]",
      addEmployee: "▶ NEI WADDA DAZUE",
      employeeName: "WADDA SEI NAAME:",
      enterName: "GANS NAAME EIGEWE",
      create: "MACHE",
      cancel: "ABBRECHE",
      delete: "AUSMACHE",
      profileManagement: "═══ PROFIL FADWALDUNG ═══",
      active: "[AKTIV]",
      select: "NEMME",
      del: "AUS",
      empSince: "WADDA SEID:",
      deleteProfile: "PROFIL AUSMACHE?",
      employee: "WADDA:",
      dataDeleted: "ALL DATE WADDE FAEMMA GLOESCHT!",
      areYouSure: "BISCHT DU SICHER?",
      yes: "YA",
      no: "NEE",
      profiles: "PROFILE",
      currentTime: "══════ YUSCHD ZEIT ══════",
      notClockedIn: "STATUS: NET EIGESTEMPELT",
      offDuty: "▬▬ NET AM SCHAFFE ▬▬",
      punchIn: "▶▶▶ EISTEMPELE ◀◀◀",
      onDuty: "═══ STATUS: AM SCHAFFE ═══",
      clockedInAt: "EIGESTEMPELT UM:",
      hours: "SCHTUNNE:",
      selectBreak: "┌─ PAAS AUSGEWWE ─┐",
      min15: "15 MIN",
      min30: "30 MIN",
      hour1: "1 SCHTUND",
      onBreak: "═══ PAAS MACHE ═══",
      endBreak: "PAAS FADICH",
      punchOut: "▶▶▶ AUSSTEMPELE ◀◀◀",
      earnings: "═══ GADDE IWWASICHT ═══",
      todayEarnings: "HEIT SEI GADDE:",
      hoursWorked: "SCHTUNNE GSCHAFFT:",
      hourlyRate: "PRO SCHTUND:",
      paySchedule: "BEZAHLE FAPLAN:",
      configSettings: "▼ EISTELLUNGE",
      hourlyRateLabel: "PRO SCHTUND ($):",
      payPeriodLabel: "BEZAHLE PERIOD:",
      language: "SCHPROOCH:",
      weekly: "YEDE WOCH",
      biWeekly: "YEDE ZWEE WOCHE",
      monthly: "YEDE MONET",
      biMonthly: "ZWEE MOL PRO MONET (15 & ENNMONET)",
      yearly: "YEDE YAAHR",
      workHistory: "═══ AWWEIT GSCHICHT ═══",
      clear: "AUSMACHE",
      date: "DADUM:",
      in: "EI:",
      out: "AUS:",
      hrs: "STD",
      weeklyNext: "Yede Woch (Nekscht:",
      biWeeklyNext: "Yede Zwee Woche (Nekscht:",
      monthlyNext: "Yede Monet (Nekscht:",
      biMonthlyNext: "Zwee Mol pro Monet (Nekscht:",
      yearlyNext: "Yede Yaahr (Nekscht:"
    },
    vi: {
      systemHeader: "═══ HỆ THỐNG THẺ CHẤM CÔNG ═══",
      appTitle: "THẺ CHẤM CÔNG",
      terminal: "MÁY CHẤM CÔNG VÀO/RA",
      selectProfile: "┌─ CHỌN HỒ SƠ NHÂN VIÊN ─┐",
      noProfiles: "[ KHÔNG TÌM THẤY HỒ SƠ ]",
      addEmployee: "▶ THÊM NHÂN VIÊN MỚI",
      employeeName: "TÊN NHÂN VIÊN:",
      enterName: "NHẬP TÊN ĐẦY ĐỦ",
      create: "TẠO",
      cancel: "HỦY",
      delete: "XÓA",
      profileManagement: "═══ QUẢN LÝ HỒ SƠ ═══",
      active: "[ĐANG HOẠT ĐỘNG]",
      select: "CHỌN",
      del: "XÓA",
      empSince: "NHÂN VIÊN TỪ:",
      deleteProfile: "XÓA HỒ SƠ?",
      employee: "NHÂN VIÊN:",
      dataDeleted: "TẤT CẢ DỮ LIỆU SẼ BỊ XÓA VĨNH VIỄN!",
      areYouSure: "BẠN CÓ CHẮC KHÔNG?",
      yes: "CÓ",
      no: "KHÔNG",
      profiles: "HỒ SƠ",
      currentTime: "══════ GIỜ HIỆN TẠI ══════",
      notClockedIn: "TRẠNG THÁI: CHƯA CHẤM CÔNG",
      offDuty: "▬▬ NGOÀI GIỜ ▬▬",
      punchIn: "▶▶▶ CHẤM CÔNG VÀO ◀◀◀",
      onDuty: "═══ TRẠNG THÁI: ĐANG LÀM VIỆC ═══",
      clockedInAt: "ĐÃ CHẤM CÔNG LÚC:",
      hours: "GIỜ:",
      selectBreak: "┌─ CHỌN THỜI GIAN NGHỈ ─┐",
      min15: "15 PHÚT",
      min30: "30 PHÚT",
      hour1: "1 GIỜ",
      onBreak: "═══ ĐANG NGHỈ ═══",
      endBreak: "KẾT THÚC NGHỈ",
      punchOut: "▶▶▶ CHẤM CÔNG RA ◀◀◀",
      earnings: "═══ TÓM TẮT THU NHẬP ═══",
      todayEarnings: "TỔNG THU NHẬP HÔM NAY:",
      hoursWorked: "GIỜ LÀM VIỆC:",
      hourlyRate: "LƯƠNG THEO GIỜ:",
      paySchedule: "LỊCH TRẢLƯƠNG:",
      configSettings: "▼ CÀI ĐẶT CẤU HÌNH",
      hourlyRateLabel: "LƯƠNG THEO GIỜ ($):",
      payPeriodLabel: "KỲ TRẢLƯƠNG:",
      language: "NGÔN NGỮ:",
      weekly: "HÀNG TUẦN",
      biWeekly: "HAI TUẦN MỘT LẦN",
      monthly: "HÀNG THÁNG",
      biMonthly: "HAI LẦN MỖI THÁNG (15 & CUỐI THÁNG)",
      yearly: "HÀNG NĂM",
      workHistory: "═══ LỊCH SỬ LÀM VIỆC ═══",
      clear: "XÓA",
      date: "NGÀY:",
      in: "VÀO:",
      out: "RA:",
      hrs: "GIỜ",
      weeklyNext: "Hàng Tuần (Kế Tiếp:",
      biWeeklyNext: "Hai Tuần (Kế Tiếp:",
      monthlyNext: "Hàng Tháng (Kế Tiếp:",
      biMonthlyNext: "Hai Lần/Tháng (Kế Tiếp:",
      yearlyNext: "Hàng Năm (Kế Tiếp:"
    },
    zh: {
      systemHeader: "═══ 员工考勤卡系统 ═══",
      appTitle: "考勤卡",
      terminal: "打卡终端",
      selectProfile: "┌─ 选择员工档案 ─┐",
      noProfiles: "[ 未找到员工档案 ]",
      addEmployee: "▶ 添加新员工",
      employeeName: "员工姓名：",
      enterName: "输入全名",
      create: "创建",
      cancel: "取消",
      delete: "删除",
      profileManagement: "═══ 档案管理 ═══",
      active: "[活跃]",
      select: "选择",
      del: "删",
      empSince: "入职日期：",
      deleteProfile: "删除档案？",
      employee: "员工：",
      dataDeleted: "所有数据将被永久删除！",
      areYouSure: "您确定吗？",
      yes: "是",
      no: "否",
      profiles: "档案",
      currentTime: "══════ 当前时间 ══════",
      notClockedIn: "状态：未打卡",
      offDuty: "▬▬ 下班 ▬▬",
      punchIn: "▶▶▶ 上班打卡 ◀◀◀",
      onDuty: "═══ 状态：上班中 ═══",
      clockedInAt: "打卡时间：",
      hours: "小时：",
      selectBreak: "┌─ 选择休息时长 ─┐",
      min15: "15分钟",
      min30: "30分钟",
      hour1: "1小时",
      onBreak: "═══ 休息中 ═══",
      endBreak: "结束休息",
      punchOut: "▶▶▶ 下班打卡 ◀◀◀",
      earnings: "═══ 收入摘要 ═══",
      todayEarnings: "今日总收入：",
      hoursWorked: "工作小时：",
      hourlyRate: "时薪：",
      paySchedule: "工资发放时间：",
      configSettings: "▼ 配置设置",
      hourlyRateLabel: "时薪 ($)：",
      payPeriodLabel: "工资周期：",
      language: "语言：",
      weekly: "每周",
      biWeekly: "双周",
      monthly: "每月",
      biMonthly: "半月（15号和月底）",
      yearly: "每年",
      workHistory: "═══ 工作历史 ═══",
      clear: "清除",
      date: "日期：",
      in: "上班：",
      out: "下班：",
      hrs: "小时",
      weeklyNext: "每周（下次：",
      biWeeklyNext: "双周（下次：",
      monthlyNext: "每月（下次：",
      biMonthlyNext: "半月（下次：",
      yearlyNext: "每年（下次："
    },
    tl: {
      systemHeader: "═══ SISTEMA NG TIME CARD ═══",
      appTitle: "TIME CARD",
      terminal: "TERMINAL NG PASOK/LABAS",
      selectProfile: "┌─ PUMILI NG PROFILE ─┐",
      noProfiles: "[ WALANG NAKITANG PROFILE ]",
      addEmployee: "▶ MAGDAGDAG NG EMPLEYADO",
      employeeName: "PANGALAN NG EMPLEYADO:",
      enterName: "ILAGAY ANG BUONG PANGALAN",
      create: "GUMAWA",
      cancel: "KANSELAHIN",
      delete: "TANGGALIN",
      profileManagement: "═══ PAMAMAHALA NG PROFILE ═══",
      active: "[AKTIBO]",
      select: "PUMILI",
      del: "TANGGAL",
      empSince: "EMPLEYADO MULA:",
      deleteProfile: "TANGGALIN ANG PROFILE?",
      employee: "EMPLEYADO:",
      dataDeleted: "LAHAT NG DATA AY PERMANENTENG MABUBURA!",
      areYouSure: "SIGURADO KA BA?",
      yes: "OO",
      no: "HINDI",
      profiles: "MGA PROFILE",
      currentTime: "══════ KASALUKUYANG ORAS ══════",
      notClockedIn: "STATUS: HINDI PA NAKA-TIME IN",
      offDuty: "▬▬ WALA SA DUTY ▬▬",
      punchIn: "▶▶▶ PUMASOK ◀◀◀",
      onDuty: "═══ STATUS: NASA DUTY ═══",
      clockedInAt: "PUMASOK SA:",
      hours: "ORAS:",
      selectBreak: "┌─ PUMILI NG TAGAL NG BREAK ─┐",
      min15: "15 MIN",
      min30: "30 MIN",
      hour1: "1 ORAS",
      onBreak: "═══ NAGPAPAHINGA ═══",
      endBreak: "TAPUSIN ANG BREAK",
      punchOut: "▶▶▶ LUMABAS ◀◀◀",
      earnings: "═══ BUOD NG KITA ═══",
      todayEarnings: "KABUUANG KITA NGAYONG ARAW:",
      hoursWorked: "ORAS NA NAGTRABAHO:",
      hourlyRate: "BAYAD BAWAT ORAS:",
      paySchedule: "ISKEDYUL NG SAHOD:",
      configSettings: "▼ MGA SETTING",
      hourlyRateLabel: "BAYAD BAWAT ORAS ($):",
      payPeriodLabel: "PANAHON NG SAHOD:",
      language: "WIKA:",
      weekly: "LINGGUHAN",
      biWeekly: "BAWAT DALAWANG LINGGO",
      monthly: "BUWANAN",
      biMonthly: "DALAWANG BESES BAWAT BUWAN (15 AT KATAPUSAN)",
      yearly: "TAUN-TAON",
      workHistory: "═══ KASAYSAYAN NG TRABAHO ═══",
      clear: "BURAHIN",
      date: "PETSA:",
      in: "PASOK:",
      out: "LABAS:",
      hrs: "ORAS",
      weeklyNext: "Lingguhan (Susunod:",
      biWeeklyNext: "Bawat 2 Linggo (Susunod:",
      monthlyNext: "Buwanan (Susunod:",
      biMonthlyNext: "2x Bawat Buwan (Susunod:",
      yearlyNext: "Taun-taon (Susunod:"
    },
    ko: {
      systemHeader: "═══ 직원 근태 카드 시스템 ═══",
      appTitle: "근태 카드",
      terminal: "출퇴근 단말기",
      selectProfile: "┌─ 직원 프로필 선택 ─┐",
      noProfiles: "[ 프로필을 찾을 수 없음 ]",
      addEmployee: "▶ 새 직원 추가",
      employeeName: "직원 이름:",
      enterName: "전체 이름 입력",
      create: "생성",
      cancel: "취소",
      delete: "삭제",
      profileManagement: "═══ 프로필 관리 ═══",
      active: "[활성]",
      select: "선택",
      del: "삭제",
      empSince: "입사일:",
      deleteProfile: "프로필 삭제?",
      employee: "직원:",
      dataDeleted: "모든 데이터가 영구적으로 삭제됩니다!",
      areYouSure: "확실합니까?",
      yes: "예",
      no: "아니오",
      profiles: "프로필",
      currentTime: "══════ 현재 시간 ══════",
      notClockedIn: "상태: 출근 전",
      offDuty: "▬▬ 근무 외 ▬▬",
      punchIn: "▶▶▶ 출근 ◀◀◀",
      onDuty: "═══ 상태: 근무 중 ═══",
      clockedInAt: "출근 시간:",
      hours: "시간:",
      selectBreak: "┌─ 휴식 시간 선택 ─┐",
      min15: "15분",
      min30: "30분",
      hour1: "1시간",
      onBreak: "═══ 휴식 중 ═══",
      endBreak: "휴식 종료",
      punchOut: "▶▶▶ 퇴근 ◀◀◀",
      earnings: "═══ 수입 요약 ═══",
      todayEarnings: "오늘 총 수입:",
      hoursWorked: "근무 시간:",
      hourlyRate: "시급:",
      paySchedule: "급여 일정:",
      configSettings: "▼ 구성 설정",
      hourlyRateLabel: "시급 ($):",
      payPeriodLabel: "급여 기간:",
      language: "언어:",
      weekly: "주간",
      biWeekly: "격주",
      monthly: "월간",
      biMonthly: "월 2회 (15일 및 말일)",
      yearly: "연간",
      workHistory: "═══ 근무 기록 ═══",
      clear: "지우기",
      date: "날짜:",
      in: "출근:",
      out: "퇴근:",
      hrs: "시간",
      weeklyNext: "주간 (다음:",
      biWeeklyNext: "격주 (다음:",
      monthlyNext: "월간 (다음:",
      biMonthlyNext: "월 2회 (다음:",
      yearlyNext: "연간 (다음:"
    },
    ar: {
      systemHeader: "═══ نظام بطاقة وقت الموظف ═══",
      appTitle: "بطاقة الوقت",
      terminal: "محطة الدخول والخروج",
      selectProfile: "┌─ اختر ملف الموظف ─┐",
      noProfiles: "[ لم يتم العثور على ملفات ]",
      addEmployee: "▶ إضافة موظف جديد",
      employeeName: "اسم الموظف:",
      enterName: "أدخل الاسم الكامل",
      create: "إنشاء",
      cancel: "إلغاء",
      delete: "حذف",
      profileManagement: "═══ إدارة الملفات ═══",
      active: "[نشط]",
      select: "اختيار",
      del: "حذف",
      empSince: "موظف منذ:",
      deleteProfile: "حذف الملف؟",
      employee: "موظف:",
      dataDeleted: "سيتم حذف جميع البيانات نهائياً!",
      areYouSure: "هل أنت متأكد؟",
      yes: "نعم",
      no: "لا",
      profiles: "الملفات",
      currentTime: "══════ الوقت الحالي ══════",
      notClockedIn: "الحالة: لم يسجل الدخول",
      offDuty: "▬▬ خارج العمل ▬▬",
      punchIn: "▶▶▶ تسجيل الدخول ◀◀◀",
      onDuty: "═══ الحالة: في العمل ═══",
      clockedInAt: "سجل الدخول في:",
      hours: "الساعات:",
      selectBreak: "┌─ اختر مدة الاستراحة ─┐",
      min15: "15 دقيقة",
      min30: "30 دقيقة",
      hour1: "ساعة واحدة",
      onBreak: "═══ في استراحة ═══",
      endBreak: "إنهاء الاستراحة",
      punchOut: "▶▶▶ تسجيل الخروج ◀◀◀",
      earnings: "═══ ملخص الأرباح ═══",
      todayEarnings: "إجمالي أرباح اليوم:",
      hoursWorked: "ساعات العمل:",
      hourlyRate: "الأجر بالساعة:",
      paySchedule: "جدول الرواتب:",
      configSettings: "▼ إعدادات التكوين",
      hourlyRateLabel: "الأجر بالساعة ($):",
      payPeriodLabel: "فترة الدفع:",
      language: "اللغة:",
      weekly: "أسبوعي",
      biWeekly: "كل أسبوعين",
      monthly: "شهري",
      biMonthly: "مرتين في الشهر (15 ونهاية الشهر)",
      yearly: "سنوي",
      workHistory: "═══ سجل العمل ═══",
      clear: "مسح",
      date: "التاريخ:",
      in: "دخول:",
      out: "خروج:",
      hrs: "ساعات",
      weeklyNext: "أسبوعي (التالي:",
      biWeeklyNext: "كل أسبوعين (التالي:",
      monthlyNext: "شهري (التالي:",
      biMonthlyNext: "مرتين/شهر (التالي:",
      yearlyNext: "سنوي (التالي:"
    },
    ru: {
      systemHeader: "═══ СИСТЕМА УЧЁТА РАБОЧЕГО ВРЕМЕНИ ═══",
      appTitle: "ТАБЕЛЬ",
      terminal: "ТЕРМИНАЛ ВХОДА/ВЫХОДА",
      selectProfile: "┌─ ВЫБЕРИТЕ ПРОФИЛЬ СОТРУДНИКА ─┐",
      noProfiles: "[ ПРОФИЛИ НЕ НАЙДЕНЫ ]",
      addEmployee: "▶ ДОБАВИТЬ НОВОГО СОТРУДНИКА",
      employeeName: "ИМЯ СОТРУДНИКА:",
      enterName: "ВВЕДИТЕ ПОЛНОЕ ИМЯ",
      create: "СОЗДАТЬ",
      cancel: "ОТМЕНА",
      delete: "УДАЛИТЬ",
      profileManagement: "═══ УПРАВЛЕНИЕ ПРОФИЛЯМИ ═══",
      active: "[АКТИВЕН]",
      select: "ВЫБРАТЬ",
      del: "УДАЛ",
      empSince: "СОТРУДНИК С:",
      deleteProfile: "УДАЛИТЬ ПРОФИЛЬ?",
      employee: "СОТРУДНИК:",
      dataDeleted: "ВСЕ ДАННЫЕ БУДУТ УДАЛЕНЫ НАВСЕГДА!",
      areYouSure: "ВЫ УВЕРЕНЫ?",
      yes: "ДА",
      no: "НЕТ",
      profiles: "ПРОФИЛИ",
      currentTime: "══════ ТЕКУЩЕЕ ВРЕМЯ ══════",
      notClockedIn: "СТАТУС: НЕ ОТМЕЧЕН",
      offDuty: "▬▬ ВНЕ РАБОТЫ ▬▬",
      punchIn: "▶▶▶ ВХОД ◀◀◀",
      onDuty: "═══ СТАТУС: НА РАБОТЕ ═══",
      clockedInAt: "ОТМЕЧЕН В:",
      hours: "ЧАСЫ:",
      selectBreak: "┌─ ВЫБЕРИТЕ ПРОДОЛЖИТЕЛЬНОСТЬ ПЕРЕРЫВА ─┐",
      min15: "15 МИН",
      min30: "30 МИН",
      hour1: "1 ЧАС",
      onBreak: "═══ НА ПЕРЕРЫВЕ ═══",
      endBreak: "ЗАКОНЧИТЬ ПЕРЕРЫВ",
      punchOut: "▶▶▶ ВЫХОД ◀◀◀",
      earnings: "═══ СВОДКА ЗАРАБОТКА ═══",
      todayEarnings: "ОБЩИЙ ЗАРАБОТОК СЕГОДНЯ:",
      hoursWorked: "ОТРАБОТАНО ЧАСОВ:",
      hourlyRate: "ПОЧАСОВАЯ СТАВКА:",
      paySchedule: "ГРАФИК ВЫПЛАТ:",
      configSettings: "▼ НАСТРОЙКИ",
      hourlyRateLabel: "ПОЧАСОВАЯ СТАВКА ($):",
      payPeriodLabel: "ПЕРИОД ОПЛАТЫ:",
      language: "ЯЗЫК:",
      weekly: "ЕЖЕНЕДЕЛЬНО",
      biWeekly: "КАЖДЫЕ ДВЕ НЕДЕЛИ",
      monthly: "ЕЖЕМЕСЯЧНО",
      biMonthly: "ДВА РАЗА В МЕСЯЦ (15 И КОНЕЦ МЕСЯЦА)",
      yearly: "ЕЖЕГОДНО",
      workHistory: "═══ ИСТОРИЯ РАБОТЫ ═══",
      clear: "ОЧИСТИТЬ",
      date: "ДАТА:",
      in: "ВХОД:",
      out: "ВЫХОД:",
      hrs: "ЧАС",
      weeklyNext: "Еженедельно (След.:",
      biWeeklyNext: "Каждые 2 нед. (След.:",
      monthlyNext: "Ежемесячно (След.:",
      biMonthlyNext: "2 раза/мес. (След.:",
      yearlyNext: "Ежегодно (След.:"
    },
    pt: {
      systemHeader: "═══ SISTEMA DE CARTÃO DE PONTO ═══",
      appTitle: "CARTÃO DE PONTO",
      terminal: "TERMINAL DE ENTRADA/SAÍDA",
      selectProfile: "┌─ SELECIONAR PERFIL DO FUNCIONÁRIO ─┐",
      noProfiles: "[ NENHUM PERFIL ENCONTRADO ]",
      addEmployee: "▶ ADICIONAR NOVO FUNCIONÁRIO",
      employeeName: "NOME DO FUNCIONÁRIO:",
      enterName: "DIGITE O NOME COMPLETO",
      create: "CRIAR",
      cancel: "CANCELAR",
      delete: "EXCLUIR",
      profileManagement: "═══ GERENCIAMENTO DE PERFIS ═══",
      active: "[ATIVO]",
      select: "SELECIONAR",
      del: "EXCL",
      empSince: "FUNCIONÁRIO DESDE:",
      deleteProfile: "EXCLUIR PERFIL?",
      employee: "FUNCIONÁRIO:",
      dataDeleted: "TODOS OS DADOS SERÃO EXCLUÍDOS PERMANENTEMENTE!",
      areYouSure: "TEM CERTEZA?",
      yes: "SIM",
      no: "NÃO",
      profiles: "PERFIS",
      currentTime: "══════ HORA ATUAL ══════",
      notClockedIn: "STATUS: NÃO REGISTRADO",
      offDuty: "▬▬ FORA DE SERVIÇO ▬▬",
      punchIn: "▶▶▶ ENTRADA ◀◀◀",
      onDuty: "═══ STATUS: EM SERVIÇO ═══",
      clockedInAt: "REGISTRADO ÀS:",
      hours: "HORAS:",
      selectBreak: "┌─ SELECIONAR DURAÇÃO DO INTERVALO ─┐",
      min15: "15 MIN",
      min30: "30 MIN",
      hour1: "1 HORA",
      onBreak: "═══ EM INTERVALO ═══",
      endBreak: "FINALIZAR INTERVALO",
      punchOut: "▶▶▶ SAÍDA ◀◀◀",
      earnings: "═══ RESUMO DE GANHOS ═══",
      todayEarnings: "GANHOS TOTAIS DE HOJE:",
      hoursWorked: "HORAS TRABALHADAS:",
      hourlyRate: "TAXA POR HORA:",
      paySchedule: "CRONOGRAMA DE PAGAMENTO:",
      configSettings: "▼ CONFIGURAÇÕES",
      hourlyRateLabel: "TAXA POR HORA ($):",
      payPeriodLabel: "PERÍODO DE PAGAMENTO:",
      language: "IDIOMA:",
      weekly: "SEMANAL",
      biWeekly: "QUINZENAL",
      monthly: "MENSAL",
      biMonthly: "BIMENSAL (DIA 15 E FIM DO MÊS)",
      yearly: "ANUAL",
      workHistory: "═══ HISTÓRICO DE TRABALHO ═══",
      clear: "LIMPAR",
      date: "DATA:",
      in: "ENTRADA:",
      out: "SAÍDA:",
      hrs: "HRS",
      weeklyNext: "Semanal (Próximo:",
      biWeeklyNext: "Quinzenal (Próximo:",
      monthlyNext: "Mensal (Próximo:",
      biMonthlyNext: "Bimensal (Próximo:",
      yearlyNext: "Anual (Próximo:"
    },
    pl: {
      systemHeader: "═══ SYSTEM KARTY CZASU PRACY ═══",
      appTitle: "KARTA CZASU PRACY",
      terminal: "TERMINAL WEJŚCIA/WYJŚCIA",
      selectProfile: "┌─ WYBIERZ PROFIL PRACOWNIKA ─┐",
      noProfiles: "[ NIE ZNALEZIONO PROFILI ]",
      addEmployee: "▶ DODAJ NOWEGO PRACOWNIKA",
      employeeName: "IMIĘ I NAZWISKO PRACOWNIKA:",
      enterName: "WPROWADŹ PEŁNE IMIĘ I NAZWISKO",
      create: "UTWÓRZ",
      cancel: "ANULUJ",
      delete: "USUŃ",
      profileManagement: "═══ ZARZĄDZANIE PROFILAMI ═══",
      active: "[AKTYWNY]",
      select: "WYBIERZ",
      del: "USUŃ",
      empSince: "PRACOWNIK OD:",
      deleteProfile: "USUNĄĆ PROFIL?",
      employee: "PRACOWNIK:",
      dataDeleted: "WSZYSTKIE DANE ZOSTANĄ TRWALE USUNIĘTE!",
      areYouSure: "CZY JESTEŚ PEWIEN?",
      yes: "TAK",
      no: "NIE",
      profiles: "PROFILE",
      currentTime: "══════ AKTUALNY CZAS ══════",
      notClockedIn: "STATUS: NIE ZALOGOWANY",
      offDuty: "▬▬ PO SŁUŻBIE ▬▬",
      punchIn: "▶▶▶ WEJŚCIE ◀◀◀",
      onDuty: "═══ STATUS: W SŁUŻBIE ═══",
      clockedInAt: "ZALOGOWANO O:",
      hours: "GODZINY:",
      selectBreak: "┌─ WYBIERZ CZAS PRZERWY ─┐",
      min15: "15 MIN",
      min30: "30 MIN",
      hour1: "1 GODZINA",
      onBreak: "═══ NA PRZERWIE ═══",
      endBreak: "ZAKOŃCZ PRZERWĘ",
      punchOut: "▶▶▶ WYJŚCIE ◀◀◀",
      earnings: "═══ PODSUMOWANIE ZAROBKÓW ═══",
      todayEarnings: "CAŁKOWITE ZAROBKI DZISIAJ:",
      hoursWorked: "PRZEPRACOWANE GODZINY:",
      hourlyRate: "STAWKA GODZINOWA:",
      paySchedule: "HARMONOGRAM WYPŁAT:",
      configSettings: "▼ USTAWIENIA KONFIGURACJI",
      hourlyRateLabel: "STAWKA GODZINOWA ($):",
      payPeriodLabel: "OKRES PŁATNOŚCI:",
      language: "JĘZYK:",
      weekly: "TYGODNIOWO",
      biWeekly: "CO DWA TYGODNIE",
      monthly: "MIESIĘCZNIE",
      biMonthly: "DWA RAZY W MIESIĄCU (15 I KONIEC MIESIĄCA)",
      yearly: "ROCZNIE",
      workHistory: "═══ HISTORIA PRACY ═══",
      clear: "WYCZYŚĆ",
      date: "DATA:",
      in: "WEJŚCIE:",
      out: "WYJŚCIE:",
      hrs: "GODZ",
      weeklyNext: "Tygodniowo (Następny:",
      biWeeklyNext: "Co 2 tyg. (Następny:",
      monthlyNext: "Miesięcznie (Następny:",
      biMonthlyNext: "2x/mies. (Następny:",
      yearlyNext: "Rocznie (Następny:"
    },
    fr: {
      systemHeader: "═══ SYSTÈME DE CARTE DE POINTAGE ═══",
      appTitle: "CARTE DE POINTAGE",
      terminal: "TERMINAL D'ENTRÉE/SORTIE",
      selectProfile: "┌─ SÉLECTIONNER LE PROFIL D'EMPLOYÉ ─┐",
      noProfiles: "[ AUCUN PROFIL TROUVÉ ]",
      addEmployee: "▶ AJOUTER UN NOUVEL EMPLOYÉ",
      employeeName: "NOM DE L'EMPLOYÉ:",
      enterName: "ENTRER LE NOM COMPLET",
      create: "CRÉER",
      cancel: "ANNULER",
      delete: "SUPPRIMER",
      profileManagement: "═══ GESTION DES PROFILS ═══",
      active: "[ACTIF]",
      select: "SÉLECTIONNER",
      del: "SUPP",
      empSince: "EMPLOYÉ DEPUIS:",
      deleteProfile: "SUPPRIMER LE PROFIL?",
      employee: "EMPLOYÉ:",
      dataDeleted: "TOUTES LES DONNÉES SERONT SUPPRIMÉES DÉFINITIVEMENT!",
      areYouSure: "ÊTES-VOUS SÛR?",
      yes: "OUI",
      no: "NON",
      profiles: "PROFILS",
      currentTime: "══════ HEURE ACTUELLE ══════",
      notClockedIn: "STATUT: NON POINTÉ",
      offDuty: "▬▬ HORS SERVICE ▬▬",
      punchIn: "▶▶▶ POINTER À L'ENTRÉE ◀◀◀",
      onDuty: "═══ STATUT: EN SERVICE ═══",
      clockedInAt: "POINTÉ À:",
      hours: "HEURES:",
      selectBreak: "┌─ SÉLECTIONNER LA DURÉE DE LA PAUSE ─┐",
      min15: "15 MIN",
      min30: "30 MIN",
      hour1: "1 HEURE",
      onBreak: "═══ EN PAUSE ═══",
      endBreak: "TERMINER LA PAUSE",
      punchOut: "▶▶▶ POINTER À LA SORTIE ◀◀◀",
      earnings: "═══ RÉSUMÉ DES GAINS ═══",
      todayEarnings: "GAINS TOTAUX AUJOURD'HUI:",
      hoursWorked: "HEURES TRAVAILLÉES:",
      hourlyRate: "TAUX HORAIRE:",
      paySchedule: "CALENDRIER DE PAIE:",
      configSettings: "▼ PARAMÈTRES DE CONFIGURATION",
      hourlyRateLabel: "TAUX HORAIRE ($):",
      payPeriodLabel: "PÉRIODE DE PAIE:",
      language: "LANGUE:",
      weekly: "HEBDOMADAIRE",
      biWeekly: "BIHEBDOMADAIRE",
      monthly: "MENSUEL",
      biMonthly: "BIMENSUEL (15 ET FIN DU MOIS)",
      yearly: "ANNUEL",
      workHistory: "═══ HISTORIQUE DE TRAVAIL ═══",
      clear: "EFFACER",
      date: "DATE:",
      in: "ENTRÉE:",
      out: "SORTIE:",
      hrs: "HRS",
      weeklyNext: "Hebdomadaire (Prochain:",
      biWeeklyNext: "Bihebdomadaire (Prochain:",
      monthlyNext: "Mensuel (Prochain:",
      biMonthlyNext: "Bimensuel (Prochain:",
      yearlyNext: "Annuel (Prochain:"
    },
    de: {
      systemHeader: "═══ ZEITKARTEN SYSTEM ═══",
      appTitle: "ZEITKARTE",
      terminal: "EIN-/AUSCHECK TERMINAL",
      selectProfile: "┌─ MITARBEITERPROFIL WÄHLEN ─┐",
      noProfiles: "[ KEINE PROFILE GEFUNDEN ]",
      addEmployee: "▶ NEUEN MITARBEITER HINZUFÜGEN",
      employeeName: "NAME DES MITARBEITERS:",
      enterName: "VOLLSTÄNDIGEN NAMEN EINGEBEN",
      create: "ERSTELLEN",
      cancel: "ABBRECHEN",
      delete: "LÖSCHEN",
      profileManagement: "═══ PROFILVERWALTUNG ═══",
      active: "[AKTIV]",
      select: "WÄHLEN",
      del: "LÖSCH",
      empSince: "MITARBEITER SEIT:",
      deleteProfile: "PROFIL LÖSCHEN?",
      employee: "MITARBEITER:",
      dataDeleted: "ALLE DATEN WERDEN DAUERHAFT GELÖSCHT!",
      areYouSure: "SIND SIE SICHER?",
      yes: "JA",
      no: "NEIN",
      profiles: "PROFILE",
      currentTime: "══════ AKTUELLE ZEIT ══════",
      notClockedIn: "STATUS: NICHT EINGESTEMPELT",
      offDuty: "▬▬ NICHT IM DIENST ▬▬",
      punchIn: "▶▶▶ EINSTEMPELN ◀◀◀",
      onDuty: "═══ STATUS: IM DIENST ═══",
      clockedInAt: "EINGESTEMPELT UM:",
      hours: "STUNDEN:",
      selectBreak: "┌─ PAUSENDAUER WÄHLEN ─┐",
      min15: "15 MIN",
      min30: "30 MIN",
      hour1: "1 STUNDE",
      onBreak: "═══ IN DER PAUSE ═══",
      endBreak: "PAUSE BEENDEN",
      punchOut: "▶▶▶ AUSSTEMPELN ◀◀◀",
      earnings: "═══ VERDIENSTÜBERSICHT ═══",
      todayEarnings: "HEUTIGE GESAMTVERDIENSTE:",
      hoursWorked: "GELEISTETE STUNDEN:",
      hourlyRate: "STUNDENLOHN:",
      paySchedule: "ZAHLUNGSPLAN:",
      configSettings: "▼ KONFIGURATIONSEINSTELLUNGEN",
      hourlyRateLabel: "STUNDENLOHN ($):",
      payPeriodLabel: "ZAHLUNGSZEITRAUM:",
      language: "SPRACHE:",
      weekly: "WÖCHENTLICH",
      biWeekly: "ZWEIWÖCHENTLICH",
      monthly: "MONATLICH",
      biMonthly: "ZWEIMAL MONATLICH (15. & MONATSENDE)",
      yearly: "JÄHRLICH",
      workHistory: "═══ ARBEITSHISTORIE ═══",
      clear: "LÖSCHEN",
      date: "DATUM:",
      in: "EIN:",
      out: "AUS:",
      hrs: "STD",
      weeklyNext: "Wöchentlich (Nächste:",
      biWeeklyNext: "Zweiwöchentlich (Nächste:",
      monthlyNext: "Monatlich (Nächste:",
      biMonthlyNext: "Zweimal/Monat (Nächste:",
      yearlyNext: "Jährlich (Nächste:"
    },
    it: {
      systemHeader: "═══ SISTEMA CARTELLINO PRESENZE ═══",
      appTitle: "CARTELLINO",
      terminal: "TERMINALE ENTRATA/USCITA",
      selectProfile: "┌─ SELEZIONA PROFILO DIPENDENTE ─┐",
      noProfiles: "[ NESSUN PROFILO TROVATO ]",
      addEmployee: "▶ AGGIUNGI NUOVO DIPENDENTE",
      employeeName: "NOME DIPENDENTE:",
      enterName: "INSERISCI NOME COMPLETO",
      create: "CREA",
      cancel: "ANNULLA",
      delete: "ELIMINA",
      profileManagement: "═══ GESTIONE PROFILI ═══",
      active: "[ATTIVO]",
      select: "SELEZIONA",
      del: "ELIM",
      empSince: "DIPENDENTE DAL:",
      deleteProfile: "ELIMINARE PROFILO?",
      employee: "DIPENDENTE:",
      dataDeleted: "TUTTI I DATI SARANNO ELIMINATI PERMANENTEMENTE!",
      areYouSure: "SEI SICURO?",
      yes: "SÌ",
      no: "NO",
      profiles: "PROFILI",
      currentTime: "══════ ORA ATTUALE ══════",
      notClockedIn: "STATO: NON TIMBRATO",
      offDuty: "▬▬ FUORI SERVIZIO ▬▬",
      punchIn: "▶▶▶ ENTRATA ◀◀◀",
      onDuty: "═══ STATO: IN SERVIZIO ═══",
      clockedInAt: "TIMBRATO ALLE:",
      hours: "ORE:",
      selectBreak: "┌─ SELEZIONA DURATA PAUSA ─┐",
      min15: "15 MIN",
      min30: "30 MIN",
      hour1: "1 ORA",
      onBreak: "═══ IN PAUSA ═══",
      endBreak: "TERMINA PAUSA",
      punchOut: "▶▶▶ USCITA ◀◀◀",
      earnings: "═══ RIEPILOGO GUADAGNI ═══",
      todayEarnings: "GUADAGNI TOTALI DI OGGI:",
      hoursWorked: "ORE LAVORATE:",
      hourlyRate: "TARIFFA ORARIA:",
      paySchedule: "CALENDARIO PAGAMENTI:",
      configSettings: "▼ IMPOSTAZIONI DI CONFIGURAZIONE",
      hourlyRateLabel: "TARIFFA ORARIA ($):",
      payPeriodLabel: "PERIODO DI PAGA:",
      language: "LINGUA:",
      weekly: "SETTIMANALE",
      biWeekly: "BISETTIMANALE",
      monthly: "MENSILE",
      biMonthly: "BIMENSILE (15 E FINE MESE)",
      yearly: "ANNUALE",
      workHistory: "═══ STORICO LAVORO ═══",
      clear: "CANCELLA",
      date: "DATA:",
      in: "ENTRATA:",
      out: "USCITA:",
      hrs: "ORE",
      weeklyNext: "Settimanale (Prossimo:",
      biWeeklyNext: "Bisettimanale (Prossimo:",
      monthlyNext: "Mensile (Prossimo:",
      biMonthlyNext: "Bimensile (Prossimo:",
      yearlyNext: "Annuale (Prossimo:"
    },
    ja: {
      systemHeader: "═══ 従業員タイムカードシステム ═══",
      appTitle: "タイムカード",
      terminal: "出退勤端末",
      selectProfile: "┌─ 従業員プロフィール選択 ─┐",
      noProfiles: "[ プロフィールが見つかりません ]",
      addEmployee: "▶ 新しい従業員を追加",
      employeeName: "従業員名:",
      enterName: "フルネームを入力",
      create: "作成",
      cancel: "キャンセル",
      delete: "削除",
      profileManagement: "═══ プロフィール管理 ═══",
      active: "[アクティブ]",
      select: "選択",
      del: "削除",
      empSince: "従業員登録日:",
      deleteProfile: "プロフィールを削除?",
      employee: "従業員:",
      dataDeleted: "すべてのデータが完全に削除されます！",
      areYouSure: "本当によろしいですか？",
      yes: "はい",
      no: "いいえ",
      profiles: "プロフィール",
      currentTime: "══════ 現在時刻 ══════",
      notClockedIn: "ステータス: 未打刻",
      offDuty: "▬▬ 勤務外 ▬▬",
      punchIn: "▶▶▶ 出勤 ◀◀◀",
      onDuty: "═══ ステータス: 勤務中 ═══",
      clockedInAt: "打刻時刻:",
      hours: "時間:",
      selectBreak: "┌─ 休憩時間を選択 ─┐",
      min15: "15分",
      min30: "30分",
      hour1: "1時間",
      onBreak: "═══ 休憩中 ═══",
      endBreak: "休憩終了",
      punchOut: "▶▶▶ 退勤 ◀◀◀",
      earnings: "═══ 収入サマリー ═══",
      todayEarnings: "本日の総収入:",
      hoursWorked: "労働時間:",
      hourlyRate: "時給:",
      paySchedule: "給与スケジュール:",
      configSettings: "▼ 設定",
      hourlyRateLabel: "時給 ($):",
      payPeriodLabel: "給与期間:",
      language: "言語:",
      weekly: "週払い",
      biWeekly: "隔週払い",
      monthly: "月払い",
      biMonthly: "月2回払い（15日と月末）",
      yearly: "年払い",
      workHistory: "═══ 勤務履歴 ═══",
      clear: "クリア",
      date: "日付:",
      in: "出勤:",
      out: "退勤:",
      hrs: "時間",
      weeklyNext: "週払い（次回:",
      biWeeklyNext: "隔週払い（次回:",
      monthlyNext: "月払い（次回:",
      biMonthlyNext: "月2回払い（次回:",
      yearlyNext: "年払い（次回:"
    }
  };

  const t = translations[language];

  // Load profiles on mount
  useEffect(() => {
    const savedProfiles = localStorage.getItem('profiles');
    const savedCurrentProfile = localStorage.getItem('currentProfile');
    const savedLanguage = localStorage.getItem('language');
    
    if (savedProfiles) {
      const parsedProfiles = JSON.parse(savedProfiles);
      setProfiles(parsedProfiles);
      
      if (savedCurrentProfile) {
        setCurrentProfile(savedCurrentProfile);
      }
    }
    
    if (savedLanguage) {
      setLanguage(savedLanguage);
    }
  }, []);

  // Save profiles to localStorage
  useEffect(() => {
    if (profiles.length > 0) {
      localStorage.setItem('profiles', JSON.stringify(profiles));
    }
  }, [profiles]);

  // Save current profile
  useEffect(() => {
    if (currentProfile) {
      localStorage.setItem('currentProfile', currentProfile);
    }
  }, [currentProfile]);

  // Save language
  useEffect(() => {
    localStorage.setItem('language', language);
  }, [language]);

  // Load profile data when profile changes
  useEffect(() => {
    if (currentProfile) {
      loadProfileData(currentProfile);
    }
  }, [currentProfile]);

  // Save profile data whenever it changes
  useEffect(() => {
    if (currentProfile) {
      saveProfileData();
    }
  }, [hourlyRate, payPeriod, workSessions, clockInTime, isClockedIn, currentProfile]);

  const loadProfileData = (profileId) => {
    const profileData = localStorage.getItem(`profile_${profileId}`);
    if (profileData) {
      const data = JSON.parse(profileData);
      setHourlyRate(data.hourlyRate || 15);
      setPayPeriod(data.payPeriod || 'weekly');
      setWorkSessions(data.workSessions || []);
      setIsClockedIn(data.isClockedIn || false);
      setClockInTime(data.clockInTime ? new Date(data.clockInTime) : null);
      setIsOnBreak(false);
      setBreakTimeLeft(0);
    } else {
      // New profile, set defaults
      setHourlyRate(15);
      setPayPeriod('weekly');
      setWorkSessions([]);
      setIsClockedIn(false);
      setClockInTime(null);
      setIsOnBreak(false);
      setBreakTimeLeft(0);
    }
  };

  const saveProfileData = () => {
    if (!currentProfile) return;
    
    const profileData = {
      hourlyRate,
      payPeriod,
      workSessions,
      isClockedIn,
      clockInTime: clockInTime ? clockInTime.toISOString() : null
    };
    
    localStorage.setItem(`profile_${currentProfile}`, JSON.stringify(profileData));
  };

  const createProfile = () => {
    if (!newProfileName.trim()) return;
    
    const newProfile = {
      id: Date.now().toString(),
      name: newProfileName.trim(),
      createdAt: new Date().toISOString()
    };
    
    setProfiles([...profiles, newProfile]);
    setNewProfileName('');
    setShowNewProfileForm(false);
    setCurrentProfile(newProfile.id);
    setShowProfileManager(false);
  };

  const requestDeleteProfile = (profileId) => {
    setProfileToDelete(profileId);
    setShowDeleteConfirm(true);
  };

  const confirmDeleteProfile = () => {
    if (!profileToDelete) return;
    
    // Remove profile data
    localStorage.removeItem(`profile_${profileToDelete}`);
    
    // Remove from profiles list
    const updatedProfiles = profiles.filter(p => p.id !== profileToDelete);
    setProfiles(updatedProfiles);
    
    // If deleting current profile, switch to another or clear
    if (currentProfile === profileToDelete) {
      if (updatedProfiles.length > 0) {
        setCurrentProfile(updatedProfiles[0].id);
      } else {
        setCurrentProfile(null);
        localStorage.removeItem('currentProfile');
      }
    }
    
    // Close modal
    setShowDeleteConfirm(false);
    setProfileToDelete(null);
  };

  const cancelDeleteProfile = () => {
    setShowDeleteConfirm(false);
    setProfileToDelete(null);
  };

  const switchProfile = (profileId) => {
    setCurrentProfile(profileId);
    setShowProfileManager(false);
  };

  // Update current time every second
  useEffect(() => {
    const timer = setInterval(() => setCurrentTime(new Date()), 1000);
    return () => clearInterval(timer);
  }, []);

  // Break timer countdown
  useEffect(() => {
    if (isOnBreak && breakTimeLeft > 0) {
      breakIntervalRef.current = setInterval(() => {
        setBreakTimeLeft(prev => {
          if (prev <= 1) {
            setIsOnBreak(false);
            clearInterval(breakIntervalRef.current);
            return 0;
          }
          return prev - 1;
        });
      }, 1000);
    }
    return () => {
      if (breakIntervalRef.current) {
        clearInterval(breakIntervalRef.current);
      }
    };
  }, [isOnBreak, breakTimeLeft]);

  const handleClockIn = () => {
    const now = new Date();
    setClockInTime(now);
    setIsClockedIn(true);
  };

  const handleClockOut = () => {
    if (!clockInTime) return;
    
    const now = new Date();
    const session = {
      clockIn: clockInTime.toISOString(),
      clockOut: now.toISOString(),
      hoursWorked: (now - clockInTime) / (1000 * 60 * 60),
      date: now.toLocaleDateString()
    };
    
    setWorkSessions([session, ...workSessions]);
    setIsClockedIn(false);
    setClockInTime(null);
    setIsOnBreak(false);
    setBreakTimeLeft(0);
    if (breakIntervalRef.current) {
      clearInterval(breakIntervalRef.current);
    }
  };

  const startBreak = (minutes) => {
    setBreakTimer(minutes);
    setBreakTimeLeft(minutes * 60);
    setIsOnBreak(true);
  };

  const cancelBreak = () => {
    setIsOnBreak(false);
    setBreakTimeLeft(0);
    if (breakIntervalRef.current) {
      clearInterval(breakIntervalRef.current);
    }
  };

  const getCurrentShiftHours = () => {
    if (!isClockedIn || !clockInTime) return 0;
    return (currentTime - clockInTime) / (1000 * 60 * 60);
  };

  const getTodayHours = () => {
    const today = new Date().toLocaleDateString();
    const todaySessions = workSessions.filter(s => s.date === today);
    const sessionHours = todaySessions.reduce((sum, s) => sum + s.hoursWorked, 0);
    const currentShift = isClockedIn ? getCurrentShiftHours() : 0;
    return sessionHours + currentShift;
  };

  const getTodayPay = () => {
    return getTodayHours() * hourlyRate;
  };

  const getPayPeriodInfo = () => {
    const today = new Date();
    const dayOfWeek = today.getDay();
    const dayOfMonth = today.getDate();
    
    let nextPayday = new Date(today);
    
    switch (payPeriod) {
      case 'weekly':
        const daysUntilFriday = (5 - dayOfWeek + 7) % 7 || 7;
        nextPayday.setDate(dayOfMonth + daysUntilFriday);
        return `${t.weeklyNext} ${nextPayday.toLocaleDateString()})`;
      case 'bi-weekly':
        const daysUntilBiWeekly = ((5 - dayOfWeek + 7) % 7 || 7) + (Math.floor((today - new Date(today.getFullYear(), 0, 1)) / (1000 * 60 * 60 * 24 * 7)) % 2 === 0 ? 0 : 7);
        nextPayday.setDate(dayOfMonth + daysUntilBiWeekly);
        return `${t.biWeeklyNext} ${nextPayday.toLocaleDateString()})`;
      case 'monthly':
        if (dayOfMonth > 1) {
          nextPayday.setMonth(nextPayday.getMonth() + 1, 1);
        } else {
          nextPayday.setDate(1);
        }
        return `${t.monthlyNext} ${nextPayday.toLocaleDateString()})`;
      case 'bi-monthly':
        if (dayOfMonth <= 15) {
          nextPayday.setDate(15);
        } else {
          nextPayday.setMonth(nextPayday.getMonth() + 1, 1);
        }
        return `${t.biMonthlyNext} ${nextPayday.toLocaleDateString()})`;
      case 'yearly':
        nextPayday = new Date(today.getFullYear(), 11, 31);
        if (dayOfMonth === 31 && today.getMonth() === 11) {
          nextPayday.setFullYear(nextPayday.getFullYear() + 1);
        }
        return `${t.yearlyNext} ${nextPayday.toLocaleDateString()})`;
      default:
        return t.weekly;
    }
  };

  const formatTime = (date) => {
    return date.toLocaleTimeString('en-US', { hour: '2-digit', minute: '2-digit' });
  };

  const formatBreakTime = (seconds) => {
    const mins = Math.floor(seconds / 60);
    const secs = seconds % 60;
    return `${mins}:${secs.toString().padStart(2, '0')}`;
  };

  const clearHistory = () => {
    if (confirm('Are you sure you want to clear all work history?')) {
      setWorkSessions([]);
    }
  };

  return (
    <div style={{ 
      minHeight: '100vh', 
      background: '#e8e5dc',
      backgroundImage: `
        repeating-linear-gradient(
          0deg,
          transparent,
          transparent 29px,
          #d4d1c8 29px,
          #d4d1c8 30px
        )
      `,
      padding: '20px',
      fontFamily: '"Courier New", Courier, monospace'
    }}>
      <div style={{ maxWidth: '600px', margin: '0 auto' }}>
        {/* Profile Selection Screen */}
        {!currentProfile ? (
          <div style={{
            background: '#f9f7f1',
            borderRadius: '4px',
            padding: '40px',
            boxShadow: '0 2px 8px rgba(0,0,0,0.15), inset 0 1px 0 rgba(255,255,255,0.5)',
            border: '1px solid #ccc',
            textAlign: 'center'
          }}>
            <div style={{
              borderBottom: '3px double #333',
              paddingBottom: '20px',
              marginBottom: '30px'
            }}>
              <div style={{ fontSize: '14px', color: '#666', marginBottom: '5px', letterSpacing: '2px' }}>
                {t.systemHeader}
              </div>
              <h1 style={{ fontSize: '32px', color: '#1a1a1a', margin: '10px 0', fontWeight: '700', letterSpacing: '1px' }}>
                {t.appTitle}
              </h1>
              <div style={{ fontSize: '12px', color: '#666', letterSpacing: '1px' }}>
                {t.terminal}
              </div>
            </div>
            
            <div style={{ 
              background: '#fff',
              border: '2px solid #333',
              padding: '15px',
              marginBottom: '30px',
              textAlign: 'left'
            }}>
              <div style={{ fontSize: '13px', fontWeight: 'bold', marginBottom: '10px', color: '#1a1a1a' }}>
                {t.selectProfile}
              </div>
            
            {profiles.length > 0 ? (
              <div style={{ marginBottom: '20px' }}>
                {profiles.map((profile, index) => (
                  <div
                    key={profile.id}
                    style={{
                      display: 'flex',
                      alignItems: 'center',
                      justifyContent: 'space-between',
                      padding: '15px',
                      background: '#f9f7f1',
                      borderRadius: '2px',
                      marginBottom: '10px',
                      cursor: 'pointer',
                      border: '2px solid #666',
                      transition: 'all 0.2s'
                    }}
                    onClick={() => switchProfile(profile.id)}
                    onMouseEnter={(e) => {
                      e.currentTarget.style.borderColor = '#1a1a1a';
                      e.currentTarget.style.background = '#fff';
                    }}
                    onMouseLeave={(e) => {
                      e.currentTarget.style.borderColor = '#666';
                      e.currentTarget.style.background = '#f9f7f1';
                    }}
                  >
                    <div style={{ display: 'flex', alignItems: 'center', gap: '15px' }}>
                      <div style={{ 
                        width: '40px', 
                        height: '40px', 
                        background: '#1a1a1a',
                        color: '#fff',
                        display: 'flex',
                        alignItems: 'center',
                        justifyContent: 'center',
                        fontSize: '18px',
                        fontWeight: 'bold',
                        border: '2px solid #000'
                      }}>
                        {String(index + 1).padStart(2, '0')}
                      </div>
                      <div style={{ textAlign: 'left' }}>
                        <div style={{ fontSize: '16px', fontWeight: '700', color: '#1a1a1a', letterSpacing: '0.5px' }}>
                          {profile.name.toUpperCase()}
                        </div>
                        <div style={{ fontSize: '11px', color: '#666' }}>
                          {t.empSince} {new Date(profile.createdAt).toLocaleDateString()}
                        </div>
                      </div>
                    </div>
                    <button
                      onClick={(e) => {
                        e.stopPropagation();
                        requestDeleteProfile(profile.id);
                      }}
                      style={{
                        padding: '8px 12px',
                        background: '#d32f2f',
                        border: '2px solid #b71c1c',
                        cursor: 'pointer',
                        color: '#fff',
                        fontSize: '11px',
                        fontWeight: 'bold',
                        letterSpacing: '0.5px'
                      }}
                    >
                      {t.delete}
                    </button>
                  </div>
                ))}
              </div>
            ) : (
              <p style={{ color: '#666', marginBottom: '20px', fontSize: '13px', textAlign: 'center' }}>
                {t.noProfiles}
              </p>
            )}
            </div>
            
            {!showNewProfileForm ? (
              <button
                onClick={() => setShowNewProfileForm(true)}
                style={{
                  width: '100%',
                  padding: '18px',
                  fontSize: '15px',
                  fontWeight: '700',
                  color: '#fff',
                  background: '#1a1a1a',
                  border: '3px solid #000',
                  cursor: 'pointer',
                  letterSpacing: '1px',
                  boxShadow: '0 4px 0 #000'
                }}
              >
                {t.addEmployee}
              </button>
            ) : (
              <div style={{ textAlign: 'left' }}>
                <label style={{ display: 'block', marginBottom: '10px', color: '#1a1a1a', fontWeight: '700', fontSize: '12px', letterSpacing: '0.5px' }}>
                  {t.employeeName}
                </label>
                <input
                  type="text"
                  value={newProfileName}
                  onChange={(e) => setNewProfileName(e.target.value)}
                  placeholder={t.enterName}
                  onKeyPress={(e) => e.key === 'Enter' && createProfile()}
                  autoFocus
                  style={{
                    width: '100%',
                    padding: '12px',
                    fontSize: '14px',
                    border: '2px solid #333',
                    background: '#fff',
                    marginBottom: '15px',
                    boxSizing: 'border-box',
                    fontFamily: '"Courier New", Courier, monospace',
                    textTransform: 'uppercase',
                    letterSpacing: '1px'
                  }}
                />
                <div style={{ display: 'flex', gap: '10px' }}>
                  <button
                    onClick={createProfile}
                    disabled={!newProfileName.trim()}
                    style={{
                      flex: 1,
                      padding: '14px',
                      fontSize: '13px',
                      fontWeight: '700',
                      color: '#fff',
                      background: newProfileName.trim() ? '#2e7d32' : '#999',
                      border: '2px solid #000',
                      cursor: newProfileName.trim() ? 'pointer' : 'not-allowed',
                      letterSpacing: '0.5px'
                    }}
                  >
                    {t.create}
                  </button>
                  <button
                    onClick={() => {
                      setShowNewProfileForm(false);
                      setNewProfileName('');
                    }}
                    style={{
                      flex: 1,
                      padding: '14px',
                      fontSize: '13px',
                      fontWeight: '700',
                      color: '#1a1a1a',
                      background: '#ddd',
                      border: '2px solid #999',
                      cursor: 'pointer',
                      letterSpacing: '0.5px'
                    }}
                  >
                    {t.cancel}
                  </button>
                </div>
              </div>
            )}
          </div>
        ) : (
          <>
            {/* Profile Manager Modal */}
            {showProfileManager && (
              <div style={{
                position: 'fixed',
                top: 0,
                left: 0,
                right: 0,
                bottom: 0,
                background: 'rgba(0,0,0,0.7)',
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'center',
                zIndex: 1000,
                padding: '20px'
              }}
              onClick={() => setShowProfileManager(false)}
              >
                <div style={{
                  background: '#f9f7f1',
                  border: '3px solid #000',
                  padding: '30px',
                  maxWidth: '500px',
                  width: '100%',
                  maxHeight: '80vh',
                  overflowY: 'auto',
                  boxShadow: '0 8px 16px rgba(0,0,0,0.3)'
                }}
                onClick={(e) => e.stopPropagation()}
                >
                  <div style={{ 
                    borderBottom: '2px solid #333',
                    paddingBottom: '15px',
                    marginBottom: '20px'
                  }}>
                    <h2 style={{ margin: 0, color: '#1a1a1a', fontSize: '20px', letterSpacing: '1px' }}>
                      {t.profileManagement}
                    </h2>
                  </div>
                  
                  {profiles.map((profile, index) => (
                    <div
                      key={profile.id}
                      style={{
                        display: 'flex',
                        alignItems: 'center',
                        justifyContent: 'space-between',
                        padding: '12px',
                        background: profile.id === currentProfile ? '#fff' : '#f0ede4',
                        border: profile.id === currentProfile ? '3px solid #1a1a1a' : '2px solid #666',
                        marginBottom: '10px'
                      }}
                    >
                      <div style={{ display: 'flex', alignItems: 'center', gap: '10px', flex: 1 }}>
                        <div style={{ 
                          width: '32px', 
                          height: '32px', 
                          background: '#1a1a1a',
                          color: '#fff',
                          display: 'flex',
                          alignItems: 'center',
                          justifyContent: 'center',
                          fontSize: '14px',
                          fontWeight: 'bold',
                          border: '2px solid #000'
                        }}>
                          {String(index + 1).padStart(2, '0')}
                        </div>
                        <div>
                          <div style={{ fontSize: '14px', fontWeight: '700', color: '#1a1a1a', letterSpacing: '0.5px' }}>
                            {profile.name.toUpperCase()}
                            {profile.id === currentProfile && (
                              <span style={{ marginLeft: '8px', fontSize: '11px', color: '#2e7d32' }}>
                                {t.active}
                              </span>
                            )}
                          </div>
                        </div>
                      </div>
                      <div style={{ display: 'flex', gap: '5px' }}>
                        {profile.id !== currentProfile && (
                          <button
                            onClick={() => switchProfile(profile.id)}
                            style={{
                              padding: '6px 12px',
                              fontSize: '11px',
                              color: '#fff',
                              background: '#1976d2',
                              border: '2px solid #0d47a1',
                              cursor: 'pointer',
                              fontWeight: 'bold',
                              letterSpacing: '0.5px'
                            }}
                          >
                            {t.select}
                          </button>
                        )}
                        <button
                          onClick={() => requestDeleteProfile(profile.id)}
                          style={{
                            padding: '6px 10px',
                            background: '#d32f2f',
                            border: '2px solid #b71c1c',
                            cursor: 'pointer',
                            color: '#fff',
                            fontSize: '11px',
                            fontWeight: 'bold'
                          }}
                        >
                          {t.del}
                        </button>
                      </div>
                    </div>
                  ))}
                  
                  <button
                    onClick={() => {
                      setShowProfileManager(false);
                      setShowNewProfileForm(true);
                      setCurrentProfile(null);
                    }}
                    style={{
                      width: '100%',
                      padding: '14px',
                      fontSize: '13px',
                      fontWeight: '700',
                      color: '#fff',
                      background: '#1a1a1a',
                      border: '3px solid #000',
                      cursor: 'pointer',
                      marginTop: '20px',
                      letterSpacing: '0.5px'
                    }}
                  >
                    {t.addEmployee}
                  </button>
                </div>
              </div>
            )}

            {/* Delete Confirmation Modal */}
            {showDeleteConfirm && (
              <div style={{
                position: 'fixed',
                top: 0,
                left: 0,
                right: 0,
                bottom: 0,
                background: 'rgba(0,0,0,0.8)',
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'center',
                zIndex: 2000,
                padding: '20px'
              }}
              onClick={cancelDeleteProfile}
              >
                <div style={{
                  background: '#f9f7f1',
                  border: '4px solid #d32f2f',
                  padding: '30px',
                  maxWidth: '400px',
                  width: '100%',
                  boxShadow: '0 8px 24px rgba(0,0,0,0.5)'
                }}
                onClick={(e) => e.stopPropagation()}
                >
                  <div style={{
                    textAlign: 'center',
                    marginBottom: '25px'
                  }}>
                    <div style={{
                      fontSize: '48px',
                      color: '#d32f2f',
                      marginBottom: '15px'
                    }}>
                      ⚠
                    </div>
                    <h2 style={{ 
                      margin: '0 0 15px 0', 
                      color: '#1a1a1a', 
                      fontSize: '20px', 
                      letterSpacing: '1px',
                      fontWeight: '700'
                    }}>
                      {t.deleteProfile}
                    </h2>
                    <div style={{
                      background: '#fff',
                      border: '2px solid #666',
                      padding: '15px',
                      marginBottom: '15px'
                    }}>
                      <div style={{ fontSize: '12px', color: '#666', marginBottom: '5px', letterSpacing: '0.5px' }}>
                        {t.employee}
                      </div>
                      <div style={{ fontSize: '16px', fontWeight: '700', color: '#1a1a1a', letterSpacing: '0.5px' }}>
                        {profiles.find(p => p.id === profileToDelete)?.name.toUpperCase()}
                      </div>
                    </div>
                    <p style={{ 
                      fontSize: '13px', 
                      color: '#d32f2f', 
                      margin: 0,
                      fontWeight: '700',
                      letterSpacing: '0.5px'
                    }}>
                      {t.dataDeleted}
                    </p>
                    <p style={{ 
                      fontSize: '14px', 
                      color: '#1a1a1a', 
                      margin: '10px 0 0 0',
                      fontWeight: '700',
                      letterSpacing: '1px'
                    }}>
                      {t.areYouSure}
                    </p>
                  </div>
                  
                  <div style={{ display: 'flex', gap: '15px' }}>
                    <button
                      onClick={confirmDeleteProfile}
                      style={{
                        flex: 1,
                        padding: '18px',
                        fontSize: '15px',
                        fontWeight: '700',
                        color: '#fff',
                        background: '#d32f2f',
                        border: '3px solid #b71c1c',
                        cursor: 'pointer',
                        letterSpacing: '1px',
                        boxShadow: '0 4px 0 #b71c1c'
                      }}
                      onMouseDown={(e) => {
                        e.currentTarget.style.transform = 'translateY(3px)';
                        e.currentTarget.style.boxShadow = '0 1px 0 #b71c1c';
                      }}
                      onMouseUp={(e) => {
                        e.currentTarget.style.transform = 'translateY(0)';
                        e.currentTarget.style.boxShadow = '0 4px 0 #b71c1c';
                      }}
                    >
                      {t.yes}
                    </button>
                    <button
                      onClick={cancelDeleteProfile}
                      style={{
                        flex: 1,
                        padding: '18px',
                        fontSize: '15px',
                        fontWeight: '700',
                        color: '#1a1a1a',
                        background: '#fff',
                        border: '3px solid #666',
                        cursor: 'pointer',
                        letterSpacing: '1px',
                        boxShadow: '0 4px 0 #666'
                      }}
                      onMouseDown={(e) => {
                        e.currentTarget.style.transform = 'translateY(3px)';
                        e.currentTarget.style.boxShadow = '0 1px 0 #666';
                      }}
                      onMouseUp={(e) => {
                        e.currentTarget.style.transform = 'translateY(0)';
                        e.currentTarget.style.boxShadow = '0 4px 0 #666';
                      }}
                    >
                      {t.no}
                    </button>
                  </div>
                </div>
              </div>
            )}

        {/* Header */}
        <div style={{ 
          display: 'flex', 
          justifyContent: 'space-between', 
          alignItems: 'center',
          marginBottom: '20px',
          background: '#f9f7f1',
          padding: '20px',
          boxShadow: '0 2px 8px rgba(0,0,0,0.15)',
          border: '2px solid #333',
          borderBottom: '4px solid #000'
        }}>
          <div>
            <div style={{ fontSize: '10px', color: '#666', marginBottom: '2px', letterSpacing: '1px' }}>
              {t.systemHeader}
            </div>
            <h1 style={{ color: '#1a1a1a', fontSize: '22px', margin: '0', fontWeight: '700', letterSpacing: '1px' }}>
              {t.appTitle}
            </h1>
            <div style={{ fontSize: '13px', color: '#333', marginTop: '5px', fontWeight: 'bold', letterSpacing: '0.5px' }}>
              {t.employee.replace(':', '')}: {profiles.find(p => p.id === currentProfile)?.name.toUpperCase()}
            </div>
          </div>
          <button
            onClick={() => setShowProfileManager(true)}
            style={{
              padding: '10px 16px',
              background: '#1a1a1a',
              border: '2px solid #000',
              cursor: 'pointer',
              color: '#fff',
              fontSize: '11px',
              fontWeight: '700',
              letterSpacing: '0.5px'
            }}
          >
            {t.profiles}
          </button>
        </div>

        {/* Current Time Display */}
        <div style={{ 
          background: '#f9f7f1', 
          padding: '25px',
          marginBottom: '20px',
          boxShadow: '0 2px 8px rgba(0,0,0,0.15)',
          border: '3px double #333',
          textAlign: 'center'
        }}>
          <div style={{ fontSize: '10px', color: '#666', marginBottom: '8px', letterSpacing: '2px' }}>
            {t.currentTime}
          </div>
          <div style={{ fontSize: '42px', fontWeight: '700', color: '#1a1a1a', marginBottom: '8px', letterSpacing: '3px' }}>
            {formatTime(currentTime)}
          </div>
          <div style={{ fontSize: '14px', color: '#333', letterSpacing: '1px' }}>
            {currentTime.toLocaleDateString('en-US', { weekday: 'long', month: 'long', day: 'numeric', year: 'numeric' }).toUpperCase()}
          </div>
        </div>

        {/* Clock In/Out Section */}
        <div style={{ 
          background: '#f9f7f1', 
          padding: '30px',
          marginBottom: '20px',
          boxShadow: '0 2px 8px rgba(0,0,0,0.15)',
          border: '2px solid #333'
        }}>
          {!isClockedIn ? (
            <>
              <div style={{ 
                textAlign: 'center',
                marginBottom: '20px',
                padding: '15px',
                background: '#fff',
                border: '2px dashed #666'
              }}>
                <div style={{ fontSize: '12px', color: '#666', marginBottom: '5px', letterSpacing: '1px' }}>
                  {t.notClockedIn}
                </div>
                <div style={{ fontSize: '24px', color: '#d32f2f', fontWeight: '700', letterSpacing: '2px' }}>
                  {t.offDuty}
                </div>
              </div>
              <button
              onClick={handleClockIn}
              style={{
                width: '100%',
                padding: '25px',
                fontSize: '20px',
                fontWeight: '700',
                color: '#fff',
                background: '#2e7d32',
                border: '4px solid #1b5e20',
                cursor: 'pointer',
                letterSpacing: '2px',
                boxShadow: '0 6px 0 #1b5e20',
                transition: 'all 0.1s'
              }}
              onMouseDown={(e) => {
                e.currentTarget.style.transform = 'translateY(4px)';
                e.currentTarget.style.boxShadow = '0 2px 0 #1b5e20';
              }}
              onMouseUp={(e) => {
                e.currentTarget.style.transform = 'translateY(0)';
                e.currentTarget.style.boxShadow = '0 6px 0 #1b5e20';
              }}
            >
              {t.punchIn}
            </button>
            </>
          ) : (
            <>
              <div style={{ 
                textAlign: 'center', 
                marginBottom: '20px',
                padding: '15px',
                background: '#e8f5e9',
                border: '3px solid #2e7d32'
              }}>
                <div style={{ fontSize: '11px', color: '#1b5e20', marginBottom: '5px', letterSpacing: '1px' }}>
                  {t.onDuty}
                </div>
                <div style={{ fontSize: '13px', color: '#666', marginBottom: '5px', letterSpacing: '0.5px' }}>
                  {t.clockedInAt}
                </div>
                <div style={{ fontSize: '28px', fontWeight: '700', color: '#1a1a1a', letterSpacing: '2px' }}>
                  {formatTime(clockInTime)}
                </div>
                <div style={{ 
                  fontSize: '16px', 
                  color: '#1a1a1a', 
                  marginTop: '12px',
                  padding: '10px',
                  background: '#fff',
                  border: '2px solid #2e7d32'
                }}>
                  <span style={{ color: '#666', fontSize: '12px', letterSpacing: '0.5px' }}>{t.hours} </span>
                  <span style={{ fontWeight: '700', fontSize: '20px' }}>{getCurrentShiftHours().toFixed(2)}</span>
                </div>
              </div>

              {/* Break Timers */}
              {!isOnBreak ? (
                <div style={{ marginBottom: '20px' }}>
                  <div style={{ 
                    fontSize: '11px', 
                    color: '#666', 
                    marginBottom: '12px', 
                    textAlign: 'center',
                    letterSpacing: '1px',
                    borderBottom: '1px solid #ccc',
                    paddingBottom: '8px'
                  }}>
                    {t.selectBreak}
                  </div>
                  <div style={{ display: 'flex', gap: '10px' }}>
                    <button
                      onClick={() => startBreak(15)}
                      style={{
                        flex: 1,
                        padding: '15px',
                        fontSize: '13px',
                        color: '#1a1a1a',
                        background: '#fff',
                        border: '3px solid #666',
                        cursor: 'pointer',
                        fontWeight: '700',
                        letterSpacing: '0.5px'
                      }}
                    >
                      {t.min15}
                    </button>
                    <button
                      onClick={() => startBreak(30)}
                      style={{
                        flex: 1,
                        padding: '15px',
                        fontSize: '13px',
                        color: '#1a1a1a',
                        background: '#fff',
                        border: '3px solid #666',
                        cursor: 'pointer',
                        fontWeight: '700',
                        letterSpacing: '0.5px'
                      }}
                    >
                      {t.min30}
                    </button>
                    <button
                      onClick={() => startBreak(60)}
                      style={{
                        flex: 1,
                        padding: '15px',
                        fontSize: '13px',
                        color: '#1a1a1a',
                        background: '#fff',
                        border: '3px solid #666',
                        cursor: 'pointer',
                        fontWeight: '700',
                        letterSpacing: '0.5px'
                      }}
                    >
                      {t.hour1}
                    </button>
                  </div>
                </div>
              ) : (
                <div style={{ 
                  background: '#fff3cd', 
                  border: '3px solid #ff9800',
                  padding: '20px',
                  marginBottom: '20px',
                  textAlign: 'center'
                }}>
                  <div style={{ fontSize: '12px', color: '#663c00', marginBottom: '5px', letterSpacing: '1px' }}>
                    {t.onBreak}
                  </div>
                  <div style={{ fontSize: '36px', fontWeight: '700', color: '#e65100', marginBottom: '10px', letterSpacing: '3px' }}>
                    {formatBreakTime(breakTimeLeft)}
                  </div>
                  <button
                    onClick={cancelBreak}
                    style={{
                      padding: '10px 24px',
                      fontSize: '12px',
                      color: '#663c00',
                      background: '#fff',
                      border: '2px solid #e65100',
                      cursor: 'pointer',
                      fontWeight: '700',
                      letterSpacing: '0.5px'
                    }}
                  >
                    {t.endBreak}
                  </button>
                </div>
              )}

              <button
                onClick={handleClockOut}
                style={{
                  width: '100%',
                  padding: '25px',
                  fontSize: '20px',
                  fontWeight: '700',
                  color: '#fff',
                  background: '#d32f2f',
                  border: '4px solid #b71c1c',
                  cursor: 'pointer',
                  letterSpacing: '2px',
                  boxShadow: '0 6px 0 #b71c1c',
                  transition: 'all 0.1s'
                }}
                onMouseDown={(e) => {
                  e.currentTarget.style.transform = 'translateY(4px)';
                  e.currentTarget.style.boxShadow = '0 2px 0 #b71c1c';
                }}
                onMouseUp={(e) => {
                  e.currentTarget.style.transform = 'translateY(0)';
                  e.currentTarget.style.boxShadow = '0 6px 0 #b71c1c';
                }}
              >
                {t.punchOut}
              </button>
            </>
          )}
        </div>

        {/* Today's Earnings */}
        <div style={{ 
          background: '#f9f7f1', 
          padding: '25px',
          marginBottom: '20px',
          boxShadow: '0 2px 8px rgba(0,0,0,0.15)',
          border: '2px solid #333'
        }}>
          <div style={{ 
            borderBottom: '2px solid #333',
            paddingBottom: '10px',
            marginBottom: '15px'
          }}>
            <div style={{ fontSize: '11px', color: '#666', letterSpacing: '1px', marginBottom: '3px' }}>
              {t.earnings}
            </div>
          </div>
          <div style={{ 
            background: '#fff',
            padding: '20px',
            border: '3px double #2e7d32',
            marginBottom: '15px'
          }}>
            <div style={{ fontSize: '12px', color: '#666', marginBottom: '5px', letterSpacing: '0.5px' }}>
              {t.todayEarnings}
            </div>
            <div style={{ fontSize: '38px', fontWeight: '700', color: '#1b5e20', letterSpacing: '2px' }}>
              ${getTodayPay().toFixed(2)}
            </div>
          </div>
          <div style={{ 
            display: 'flex',
            justifyContent: 'space-between',
            fontSize: '13px',
            padding: '12px',
            background: '#fff',
            border: '2px solid #666',
            marginBottom: '8px'
          }}>
            <span style={{ color: '#666', letterSpacing: '0.5px' }}>{t.hoursWorked}</span>
            <span style={{ fontWeight: '700', color: '#1a1a1a' }}>{getTodayHours().toFixed(2)}</span>
          </div>
          <div style={{ 
            display: 'flex',
            justifyContent: 'space-between',
            fontSize: '13px',
            padding: '12px',
            background: '#fff',
            border: '2px solid #666',
            marginBottom: '8px'
          }}>
            <span style={{ color: '#666', letterSpacing: '0.5px' }}>{t.hourlyRate}</span>
            <span style={{ fontWeight: '700', color: '#1a1a1a' }}>${hourlyRate.toFixed(2)}/hr</span>
          </div>
          <div style={{ 
            display: 'flex',
            justifyContent: 'space-between',
            fontSize: '13px',
            padding: '12px',
            background: '#fff',
            border: '2px solid #666'
          }}>
            <span style={{ color: '#666', letterSpacing: '0.5px' }}>{t.paySchedule}</span>
            <span style={{ fontWeight: '700', color: '#1a1a1a' }}>{getPayPeriodInfo()}</span>
          </div>
        </div>

        {/* Settings */}
        <div style={{ 
          background: '#f9f7f1', 
          padding: '25px',
          marginBottom: '20px',
          boxShadow: '0 2px 8px rgba(0,0,0,0.15)',
          border: '2px solid #333'
        }}>
          <button
            onClick={() => setShowSettings(!showSettings)}
            style={{
              width: '100%',
              padding: '15px',
              fontSize: '14px',
              fontWeight: '700',
              color: '#1a1a1a',
              background: '#fff',
              border: '2px solid #666',
              cursor: 'pointer',
              textAlign: 'left',
              letterSpacing: '0.5px'
            }}
          >
            {t.configSettings} {showSettings ? '[-]' : '[+]'}
          </button>
          
          {showSettings && (
            <div style={{ marginTop: '20px' }}>
              <div style={{ marginBottom: '20px' }}>
                <label style={{ 
                  display: 'block', 
                  marginBottom: '8px', 
                  color: '#1a1a1a', 
                  fontWeight: '700',
                  fontSize: '12px',
                  letterSpacing: '0.5px'
                }}>
                  {t.hourlyRateLabel}
                </label>
                <input
                  type="number"
                  value={hourlyRate}
                  onChange={(e) => setHourlyRate(parseFloat(e.target.value) || 0)}
                  step="0.50"
                  min="0"
                  style={{
                    width: '100%',
                    padding: '12px',
                    fontSize: '16px',
                    border: '2px solid #333',
                    background: '#fff',
                    boxSizing: 'border-box',
                    fontFamily: '"Courier New", Courier, monospace',
                    fontWeight: '700'
                  }}
                />
              </div>
              
              <div style={{ marginBottom: '20px' }}>
                <label style={{ 
                  display: 'block', 
                  marginBottom: '8px', 
                  color: '#1a1a1a', 
                  fontWeight: '700',
                  fontSize: '12px',
                  letterSpacing: '0.5px'
                }}>
                  {t.payPeriodLabel}
                </label>
                <select
                  value={payPeriod}
                  onChange={(e) => setPayPeriod(e.target.value)}
                  style={{
                    width: '100%',
                    padding: '12px',
                    fontSize: '14px',
                    border: '2px solid #333',
                    background: '#fff',
                    boxSizing: 'border-box',
                    fontFamily: '"Courier New", Courier, monospace',
                    fontWeight: '700'
                  }}
                >
                  <option value="weekly">{t.weekly}</option>
                  <option value="bi-weekly">{t.biWeekly}</option>
                  <option value="monthly">{t.monthly}</option>
                  <option value="bi-monthly">{t.biMonthly}</option>
                  <option value="yearly">{t.yearly}</option>
                </select>
              </div>

              <div style={{ marginBottom: '20px' }}>
                <label style={{ 
                  display: 'block', 
                  marginBottom: '8px', 
                  color: '#1a1a1a', 
                  fontWeight: '700',
                  fontSize: '12px',
                  letterSpacing: '0.5px'
                }}>
                  {t.language}
                </label>
                <select
                  value={language}
                  onChange={(e) => setLanguage(e.target.value)}
                  style={{
                    width: '100%',
                    padding: '12px',
                    fontSize: '14px',
                    border: '2px solid #333',
                    background: '#fff',
                    boxSizing: 'border-box',
                    fontFamily: '"Courier New", Courier, monospace',
                    fontWeight: '700'
                  }}
                >
                  <option value="en">ENGLISH</option>
                  <option value="es">ESPAÑOL</option>
                  <option value="ht">KREYÒL AYISYEN</option>
                  <option value="lc">KOURI-VINI (LOUISIANA CREOLE)</option>
                  <option value="pd">PENNSILFAANISCH DEITSCH</option>
                  <option value="vi">TIẾNG VIỆT</option>
                  <option value="zh">中文 (CHINESE)</option>
                  <option value="tl">TAGALOG</option>
                  <option value="ko">한국어 (KOREAN)</option>
                  <option value="ar">العربية (ARABIC)</option>
                  <option value="ru">РУССКИЙ (RUSSIAN)</option>
                  <option value="pt">PORTUGUÊS</option>
                  <option value="pl">POLSKI</option>
                  <option value="fr">FRANÇAIS</option>
                  <option value="de">DEUTSCH</option>
                  <option value="it">ITALIANO</option>
                  <option value="ja">日本語 (JAPANESE)</option>
                </select>
              </div>
            </div>
          )}
        </div>

        {/* Work History */}
        {workSessions.length > 0 && (
          <div style={{ 
            background: '#f9f7f1', 
            padding: '25px',
            boxShadow: '0 2px 8px rgba(0,0,0,0.15)',
            border: '2px solid #333'
          }}>
            <div style={{ 
              display: 'flex', 
              justifyContent: 'space-between', 
              alignItems: 'center', 
              marginBottom: '20px',
              paddingBottom: '15px',
              borderBottom: '2px solid #333'
            }}>
              <h2 style={{ 
                margin: 0, 
                fontSize: '16px', 
                color: '#1a1a1a',
                letterSpacing: '1px'
              }}>
                {t.workHistory}
              </h2>
              <button
                onClick={clearHistory}
                style={{
                  padding: '8px 16px',
                  fontSize: '11px',
                  color: '#fff',
                  background: '#d32f2f',
                  border: '2px solid #b71c1c',
                  cursor: 'pointer',
                  fontWeight: '700',
                  letterSpacing: '0.5px'
                }}
              >
                {t.clear}
              </button>
            </div>
            <div style={{ maxHeight: '300px', overflowY: 'auto' }}>
              {workSessions.map((session, index) => (
                <div 
                  key={index}
                  style={{ 
                    padding: '15px',
                    background: '#fff',
                    marginBottom: '10px',
                    border: '2px solid #666'
                  }}
                >
                  <div style={{ 
                    display: 'flex',
                    justifyContent: 'space-between',
                    marginBottom: '8px',
                    paddingBottom: '8px',
                    borderBottom: '1px dashed #ccc'
                  }}>
                    <span style={{ fontSize: '12px', fontWeight: '700', color: '#1a1a1a', letterSpacing: '0.5px' }}>
                      {t.date}
                    </span>
                    <span style={{ fontSize: '12px', fontWeight: '700', color: '#1a1a1a' }}>
                      {session.date.toUpperCase()}
                    </span>
                  </div>
                  <div style={{ 
                    display: 'flex',
                    justifyContent: 'space-between',
                    fontSize: '11px',
                    color: '#666',
                    marginBottom: '5px'
                  }}>
                    <span>{t.in} {formatTime(new Date(session.clockIn))}</span>
                    <span>{t.out} {formatTime(new Date(session.clockOut))}</span>
                  </div>
                  <div style={{ 
                    background: '#e8f5e9',
                    padding: '8px',
                    marginTop: '8px',
                    border: '1px solid #2e7d32',
                    display: 'flex',
                    justifyContent: 'space-between'
                  }}>
                    <span style={{ fontSize: '12px', color: '#1b5e20', fontWeight: '700' }}>
                      {session.hoursWorked.toFixed(2)} {t.hrs}
                    </span>
                    <span style={{ fontSize: '12px', color: '#1b5e20', fontWeight: '700' }}>
                      ${(session.hoursWorked * hourlyRate).toFixed(2)}
                    </span>
                  </div>
                </div>
              ))}
            </div>
          </div>
        )}
          </>
        )}
      </div>
    </div>
  );
}
