

pub mod navigation;
pub mod guidance;
pub mod interface;

use navigation::kalman_filter::ExtendedKalmanFilter;
use guidance::guidance_laws::ProportionalNavigation;

#[repr(C)]
pub struct FfiNavState {
    pub x: f64, pub y: f64, pub z: f64,
    pub vx: f64, pub vy: f64, pub vz: f64,
    pub q0: f64, pub q1: f64, pub q2: f64, pub q3: f64,
    pub p: f64, pub q: f64, pub r: f64,
}

#[repr(C)]
pub struct FfiGuidanceCommands {
    pub delta_pitch: f64,
    pub delta_yaw: f64,
    pub delta_roll: f64,
    pub mode_flag: u8,
}

pub struct GncContext {
    pub ekf: ExtendedKalmanFilter,
    pub pn_guidance: ProportionalNavigation,
}

#[no_mangle]
pub extern "C" fn rust_gnc_init(target_x: f64, target_y: f64, target_z: f64) -> *mut GncContext {
    let ctx = GncContext {
        ekf: ExtendedKalmanFilter::new(),
        pn_guidance: ProportionalNavigation::new(target_x, target_y, target_z),
    };
    Box::into_raw(Box::new(ctx))
}

#[no_mangle]
pub extern "C" fn rust_gnc_destroy(ctx: *mut GncContext) {
    if !ctx.is_null() {
        unsafe {
            let _ = Box::from_raw(ctx);
        }
    }
}

#[no_mangle]
pub extern "C" fn rust_gnc_update(
    ctx: *mut GncContext,
    state: *const FfiNavState,
    out_cmd: *mut FfiGuidanceCommands,
) {
    if ctx.is_null() || state.is_null() || out_cmd.is_null() {
        return;
    }

    unsafe {
        let context = &mut *ctx;
        let s = &*state;

        // 1. Run memory-safe EKF filter state update
        context.ekf.update(s.x, s.y, s.z, s.vx, s.vy, s.vz);

        // 2. Compute Proportional Navigation guidance commands
        let (pitch_cmd, yaw_cmd, roll_cmd) = context.pn_guidance.compute_command(
            s.x, s.y, s.z, s.vx, s.vy, s.vz
        );

        // 3. Populate output command FFI struct
        let cmd = &mut *out_cmd;
        cmd.delta_pitch = pitch_cmd;
        cmd.delta_yaw = yaw_cmd;
        cmd.delta_roll = roll_cmd;
        cmd.mode_flag = 1; // Active guidance tracking
    }
}