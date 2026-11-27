#include "user_global.h"

/**
  \fn          int32_t Initialize (ARM_ETH_PHY_Read_t  fn_read,
                                   ARM_ETH_PHY_Write_t fn_write)
  \brief       Initialize Ethernet PHY Device.
  \param[in]   fn_read   Pointer to \ref ARM_ETH_MAC_PHY_Read
  \param[in]   fn_write  Pointer to \ref ARM_ETH_MAC_PHY_Write
  \return      \ref execution_status
*/
static int32_t Initialize (ARM_ETH_PHY_Read_t fn_read, ARM_ETH_PHY_Write_t fn_write) {

  if ((fn_read == NULL) || (fn_write == NULL)) { return ARM_DRIVER_ERROR_PARAMETER; }

  if ((PHY.flags & PHY_INIT) == 0U) {
    /* Register PHY read/write functions. */
    PHY.reg_rd = fn_read;
    PHY.reg_wr = fn_write;

    PHY.bmcr   = 0U;
    PHY.flags  = PHY_INIT;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t Uninitialize (void)
  \brief       De-initialize Ethernet PHY Device.
  \return      \ref execution_status
*/
static int32_t Uninitialize (void) {

  PHY.reg_rd = NULL;
  PHY.reg_wr = NULL;
  PHY.bmcr   = 0U;
  PHY.flags  = 0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t PowerControl (ARM_POWER_STATE state)
  \brief       Control Ethernet PHY Device Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t PowerControl (ARM_POWER_STATE state) {
  uint16_t val;

  switch ((int32_t)state) {
    case ARM_POWER_OFF:
      if ((PHY.flags & PHY_INIT) == 0U) {
        /* Initialize must provide register access function pointers */
        return ARM_DRIVER_ERROR;
      }

      PHY.flags &= ~PHY_POWER;
      PHY.bmcr   =  BMCR_POWER_DOWN;

      return (PHY.reg_wr(ETH_PHY_ADDR, REG_BMCR, PHY.bmcr));

    case ARM_POWER_FULL:
      if ((PHY.flags & PHY_INIT) == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if (PHY.flags & PHY_POWER) {
        return ARM_DRIVER_OK;
      }

      /* Check Device Identification. */
      PHY.reg_rd(ETH_PHY_ADDR, REG_PHYIDR1, &val);

      if (val != PHY_ID1) {
        /* Invalid PHY ID */
        return ARM_DRIVER_ERROR_UNSUPPORTED;
      }

      PHY.reg_rd(ETH_PHY_ADDR, REG_PHYIDR2, &val);

      if ((val & 0xFFF0) != PHY_ID2) {
        /* Invalid PHY ID */
        return ARM_DRIVER_ERROR_UNSUPPORTED;
      }

      PHY.bmcr = 0U;

      if (PHY.reg_wr(ETH_PHY_ADDR, REG_BMCR, PHY.bmcr) != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
      }

      PHY.flags |=  PHY_POWER;

      return ARM_DRIVER_OK;

    case ARM_POWER_LOW:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
}

/**
  \fn          int32_t SetInterface (uint32_t interface)
  \brief       Set Ethernet Media Interface.
  \param[in]   interface  Media Interface type
  \return      \ref execution_status
*/
static int32_t SetInterface (uint32_t interface) {

  if ((PHY.flags & PHY_POWER) == 0U) { return ARM_DRIVER_ERROR; }

  switch (interface) {
    case ARM_ETH_INTERFACE_RMII:
      break;
    case ARM_ETH_INTERFACE_MII:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return(0);
}

/**
  \fn          int32_t SetMode (uint32_t mode)
  \brief       Set Ethernet PHY Device Operation mode.
  \param[in]   mode  Operation Mode
  \return      \ref execution_status
*/
static int32_t SetMode (uint32_t mode) {
  uint16_t val;

  if ((PHY.flags & PHY_POWER) == 0U) { return ARM_DRIVER_ERROR; }

  val = PHY.bmcr & BMCR_POWER_DOWN;

  switch (mode & ARM_ETH_PHY_SPEED_Msk) {
    case ARM_ETH_PHY_SPEED_10M:
      break;
    case ARM_ETH_PHY_SPEED_100M:
      val |=  BMCR_SPEED_SEL;
      break;
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  switch (mode & ARM_ETH_PHY_DUPLEX_Msk) {
    case ARM_ETH_PHY_DUPLEX_HALF:
      break;
    case ARM_ETH_PHY_DUPLEX_FULL:
      val |=  BMCR_DUPLEX;
      break;
  }

  if (mode & ARM_ETH_PHY_AUTO_NEGOTIATE) {
    val |= BMCR_ANEG_EN;
  }

  if (mode & ARM_ETH_PHY_LOOPBACK) {
    val |= BMCR_LOOPBACK;
  }

  if (mode & ARM_ETH_PHY_ISOLATE) {
    val |= BMCR_ISOLATE;
  }

  PHY.bmcr = val;

  return (PHY.reg_wr(ETH_PHY_ADDR, REG_BMCR, PHY.bmcr));
}

/**
  \fn          ARM_ETH_LINK_STATE GetLinkState (void)
  \brief       Get Ethernet PHY Device Link state.
  \return      current link status \ref ARM_ETH_LINK_STATE
*/
static ARM_ETH_LINK_STATE GetLinkState (void) {
  ARM_ETH_LINK_STATE state;
  uint16_t           val = 0U;

  if (PHY.flags & PHY_POWER) {
    PHY.reg_rd(ETH_PHY_ADDR, REG_BMSR, &val);
  }
  state = (val & BMSR_LINK_STAT) ? ARM_ETH_LINK_UP : ARM_ETH_LINK_DOWN;

  return (state);
}

/**
  \fn          ARM_ETH_LINK_INFO GetLinkInfo (void)
  \brief       Get Ethernet PHY Device Link information.
  \return      current link parameters \ref ARM_ETH_LINK_INFO
*/
static ARM_ETH_LINK_INFO GetLinkInfo (void) {
  ARM_ETH_LINK_INFO info;
  uint16_t          val = 0U;

  if (PHY.flags & PHY_POWER) {
    PHY.reg_rd(ETH_PHY_ADDR, REG_PSCSR, &val);
  }

  info.speed  = (val & PSCSR_SPEED)  ? ARM_ETH_SPEED_10M   : ARM_ETH_SPEED_100M;
  info.duplex = (val & PSCSR_DUPLEX) ? ARM_ETH_DUPLEX_FULL : ARM_ETH_DUPLEX_HALF;

  return (info);
}


/* PHY Driver Control Block */
extern
ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH_PHY_NUM);
ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH_PHY_NUM) = {
  GetVersion,
  Initialize,
  Uninitialize,
  PowerControl,
  SetInterface,
  SetMode,
  GetLinkState,
  GetLinkInfo
};
