LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_arith.all;
USE ieee.std_logic_unsigned.all;

ENTITY Base IS
	PORT (
		SW : IN STD_LOGIC_VECTOR(17 DOWNTO 0);
		KEY : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
		CLOCK_50 : IN STD_LOGIC;
		CLOCK_27 : IN STD_LOGIC;
		LEDG : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		DRAM_CLK, DRAM_CKE : OUT STD_LOGIC;
		DRAM_ADDR : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
		DRAM_BA_0, DRAM_BA_1 : BUFFER STD_LOGIC;
		DRAM_CS_N, DRAM_CAS_N, DRAM_RAS_N, DRAM_WE_N : OUT STD_LOGIC;
		DRAM_DQ : INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
		DRAM_UDQM, DRAM_LDQM : BUFFER STD_LOGIC ;
		LCD_DATA : inout STD_LOGIC_VECTOR(7 downto 0);
		LCD_ON, LCD_BLON, LCD_EN, LCD_RS, LCD_RW : out STD_LOGIC;
		SD_DAT, SD_DAT3,SD_CMD:INOUT std_LOGIC; 
		SD_CLK: out STD_LOGIC;
		VGA_R, VGA_G, VGA_B : out std_logic_vector(9 downto 0);
		VGA_CLK, VGA_BLANK, VGA_HS, VGA_VS, VGA_SYNC : out std_logic;
		SRAM_DQ : inout std_logic_vector(15 downto 0);
		SRAM_ADDR : out std_logic_vector(17 downto 0);
		SRAM_LB_N, SRAM_UB_N, SRAM_CE_N, SRAM_OE_N, SRAM_WE_N : out std_logic;
		I2C_SDAT : inout std_logic;
		I2C_SCLK : out std_logic;
		AUD_XCK, AUD_DACDAT : out std_logic;
		AUD_ADCDAT, AUD_ADCLRCK, AUD_BCLK, AUD_DACLRCK : in std_logic;
		GPIO_1: inout std_LOGIC_vector(36 downto 0)
	);
END Base;


ARCHITECTURE Structure OF Base IS
	COMPONENT nios_system
		PORT (
			clk_clk : IN STD_LOGIC;
			reset_reset_n : IN STD_LOGIC;
			sdram_clk_clk : OUT STD_LOGIC;
			leds_export : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
			switches_export : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
			sdram_wire_addr : OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
			sdram_wire_ba : BUFFER STD_LOGIC_VECTOR(1 DOWNTO 0);
			sdram_wire_cas_n : OUT STD_LOGIC;
			sdram_wire_cke : OUT STD_LOGIC;
			sdram_wire_cs_n : OUT STD_LOGIC;
			sdram_wire_dq : INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
			sdram_wire_dqm : BUFFER STD_LOGIC_VECTOR(1 DOWNTO 0);
			sdram_wire_ras_n : OUT STD_LOGIC;
			sdram_wire_we_n : OUT STD_LOGIC;
			lcd_data_DATA : inout STD_LOGIC_VECTOR(7 downto 0);
			lcd_data_ON : OUT STD_LOGIC;
			lcd_data_BLON : OUT STD_LOGIC;
			lcd_data_EN : OUT STD_LOGIC;
			lcd_data_RS : OUT STD_LOGIC;
			lcd_data_RW : OUT STD_LOGIC; 
			push_export : IN STD_LOGIC_VECTOR(3 downto 0);
			sd_card_b_SD_cmd : INOUT std_LOGIC;   
			sd_card_b_SD_dat : INOUT std_LOGIC; 
			sd_card_b_SD_dat3 :  INOUT std_LOGIC; 
			sd_card_o_SD_clock : OUT STD_LOGIC;
			sram_DQ              : inout std_logic_vector(15 downto 0) := (others => 'X'); 
			sram_ADDR            : out   std_logic_vector(17 downto 0);                    
			sram_LB_N            : out   std_logic;                                        
			sram_UB_N            : out   std_logic;                                        
			sram_CE_N            : out   std_logic;                                        
			sram_OE_N            : out   std_logic;                                        
			sram_WE_N            : out   std_logic;                                        
			vga_controller_CLK   : out   std_logic;                                        
			vga_controller_HS    : out   std_logic;                                        
			vga_controller_VS    : out   std_logic;                                        
			vga_controller_BLANK : out   std_logic;                                        
			vga_controller_SYNC  : out   std_logic;                                        
			vga_controller_R     : out   std_logic_vector(9 downto 0);                     
			vga_controller_G     : out   std_logic_vector(9 downto 0);                     
			vga_controller_B     : out   std_logic_vector(9 downto 0);
			spi_MISO 				: IN std_LOGIC;
			spi_MOSI					: out std_LOGIC;
			spi_SCLK 				: out std_LOGIC;
			spi_SS_n					: out std_LOGIC;
			audio_ADCDAT            : in    std_logic                     := 'X';             
			audio_ADCLRCK           : in    std_logic                     := 'X';            
			audio_BCLK              : in    std_logic                     := 'X';             
			audio_DACDAT            : out   std_logic;                                        
			audio_DACLRCK           : in    std_logic                     := 'X';             
			audio_video_config_SDAT : inout std_logic                     := 'X';             
			audio_video_config_SCLK : out   std_logic;                                       
			clocks_secondary_clk    : in    std_logic                     := 'X';             
			audio_clk_clk           : out   std_logic                                         
		);
	END COMPONENT;
	SIGNAL DQM : STD_LOGIC_VECTOR(1 DOWNTO 0);
	SIGNAL BA : STD_LOGIC_VECTOR(1 DOWNTO 0);
BEGIN
	DRAM_BA_0 <= BA(0);
	DRAM_BA_1 <= BA(1);
	DRAM_UDQM <= DQM(1);
	DRAM_LDQM <= DQM(0);
	--Instantiate the Nios II system entity generated by the Qsys tool.
	NiosII: nios_system PORT MAP (
		clk_clk => CLOCK_50,
		reset_reset_n => KEY(0),
		push_export(3 downto 1) => KEY(3 downto 1),
		sdram_clk_clk => DRAM_CLK,
		leds_export => LEDG,
		switches_export => SW(7 downto 0),
		sdram_wire_addr => DRAM_ADDR,
		sdram_wire_ba => BA,
		sdram_wire_cas_n => DRAM_CAS_N,
		sdram_wire_cke => DRAM_CKE,
		sdram_wire_cs_n => DRAM_CS_N,
		sdram_wire_dq => DRAM_DQ,
		sdram_wire_dqm => DQM,
		sdram_wire_ras_n => DRAM_RAS_N,
		sdram_wire_we_n => DRAM_WE_N,
		lcd_data_DATA => LCD_DATA,
		lcd_data_ON => LCD_ON,
		lcd_data_EN => LCD_EN,
		lcd_data_RS => LCD_RS,
		lcd_data_RW => LCD_RW,
		lcd_data_BLON => LCD_BLON,
		sd_card_b_SD_cmd => SD_CMD,   
		sd_card_b_SD_dat => SD_DAT,
		sd_card_b_SD_dat3 => SD_DAT3, 
		sd_card_o_SD_clock => SD_CLK,
		sram_DQ              => SRAM_DQ,              
		sram_ADDR            => SRAM_ADDR ,           
		sram_LB_N            => SRAM_LB_N,            
		sram_UB_N            => SRAM_UB_N,            
		sram_CE_N            => SRAM_CE_N,            
		sram_OE_N            => SRAM_OE_N,            
		sram_WE_N            => SRAM_WE_N,            
		vga_controller_CLK   => VGA_CLK,   
		vga_controller_HS    => VGA_HS,    
		vga_controller_VS    => VGA_VS,    
		vga_controller_BLANK => VGA_BLANK, 
		vga_controller_SYNC  => VGA_SYNC,  
		vga_controller_R     => VGA_R,     
		vga_controller_G     => VGA_G,     
		vga_controller_B     => VGA_B,
		spi_MISO => GPIO_1(0),
		spi_MOSI => GPIO_1(1),
		spi_SS_n => GPIO_1(2),
		spi_SCLK => GPIO_1(4),
		audio_ADCDAT            => AUD_ADCDAT,           
		audio_ADCLRCK           => AUD_ADCLRCK,         
		audio_BCLK              => AUD_BCLK,           
		audio_DACDAT            => AUD_DACDAT,         
		audio_DACLRCK           => AUD_DACLRCK,        
		audio_video_config_SDAT => I2C_SDAT,
		audio_video_config_SCLK => I2C_SCLK,
		clocks_secondary_clk    => CLOCK_27,  
		audio_clk_clk           => AUD_XCK           
	);
END Structure;