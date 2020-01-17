[@bs.deriving abstract]
type t('a) = {
  frame: int,
  notification: Rx_Notification.t('a),
  isGhost: bool
};