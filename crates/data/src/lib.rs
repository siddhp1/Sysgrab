use serde::{Deserialize, Serialize};

#[derive(Clone, Copy, Debug, Deserialize, Eq, PartialEq, Serialize)]
pub enum DataPointKind {
    Os,
    Architecture,
    Kernel,
    Computer,
    Shell,
    Uptime,
    Cpu,
    Memory,
}

#[derive(Debug)]
pub struct DataPoint {
    pub kind: DataPointKind,
    pub value: String,
}
