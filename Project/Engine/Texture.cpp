#include "EnginePCH.h"
#include "Texture.h"

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/Debug/DirectXTex.lib") 
#else 
#pragma comment(lib, "DirectXTex/Release/DirectXTex.lib") 
#endif


#include "Func.h"
#include "PathMgr.h"




namespace mh
{
	namespace stdfs = std::filesystem;

	Texture::Texture()
		: IRes(eResourceType::Texture)
		, mDesc()
		, mTexture()
		, mImage()
		, mSRV()
		, mUAV()

		, mDSV()
		, mRTV()

		, mCurBoundView()
		, mCurBoundRegister(-1)
		, mCurBoundStage(eShaderStageFlag::NONE)
	{
	}

	Texture::~Texture()
	{

	}

	void Texture::Clear(UINT _startSlot)
	{
		ID3D11ShaderResourceView* srv = nullptr;

		auto pContext = GPUMgr::GetInst()->GetContext();
		pContext->VSSetShaderResources(_startSlot, 1u, &srv);
		pContext->HSSetShaderResources(_startSlot, 1u, &srv);
		pContext->DSSetShaderResources(_startSlot, 1u, &srv);
		pContext->GSSetShaderResources(_startSlot, 1u, &srv);
		pContext->PSSetShaderResources(_startSlot, 1u, &srv);
		pContext->CSSetShaderResources(_startSlot, 1u, &srv);
	}

	bool Texture::Create(UINT _width, UINT _height, DXGI_FORMAT _pixelFormat, UINT _D3D11_BIND_FLAG, D3D11_USAGE _Usage)
	{
		//Depth stencil _texture
		mDesc.BindFlags = _D3D11_BIND_FLAG;
		mDesc.Usage = _Usage;

		//CPU의 읽기/쓰기 가능 여부를 설정
		if (D3D11_USAGE::D3D11_USAGE_DYNAMIC == _Usage)
			mDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		else if (D3D11_USAGE::D3D11_USAGE_STAGING == _Usage)
			mDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		else
			mDesc.CPUAccessFlags = 0;

		mDesc.Format = _pixelFormat;
		mDesc.Width = _width;
		mDesc.Height = _height;
		mDesc.ArraySize = 1;
		mDesc.MiscFlags = 0;

		//원본만 생성.(자세한 내용은 밉맵을 찾아볼 것)
		mDesc.MipLevels = 1;
		mDesc.SampleDesc.Count = 1;
		mDesc.SampleDesc.Quality = 0;
		

		bool bResult = false;
		auto pDevice = GPUMgr::GetInst()->GetDevice();
		bResult = SUCCEEDED(pDevice->CreateTexture2D(&mDesc, nullptr, mTexture.GetAddressOf()));

		if(false == bResult)
		{
			ERROR_MESSAGE_W(L"텍스처 생성에 실패 했습니다.");
			mDesc = {};
			return false;
		}
		
		bResult = CreateView();
		if (false == bResult)
		{
			mTexture = nullptr;
			mDesc = {};
		}

		return bResult;
	}

	bool Texture::Create(Microsoft::WRL::ComPtr<ID3D11Texture2D> _texture)
	{
		mTexture = _texture;
		mTexture->GetDesc(&mDesc);

		bool Result = CreateView();
		
		if (false == Result)
		{
			mTexture = nullptr;
			mDesc = {};
		}

		return Result;
	}

	bool Texture::Create(const D3D11_TEXTURE2D_DESC& _TexDesc)
	{
		bool Result = false;
		mDesc = _TexDesc;

		Result = SUCCEEDED(GPUMgr::GetInst()->GetDevice()->CreateTexture2D(&mDesc, nullptr, mTexture.GetAddressOf()));
		if (false == Result)
		{
			ERROR_MESSAGE_W(L"텍스처 생성에 실패 했습니다.");
			mDesc = {};
			return false;
		}

		Result = CreateView();
		if(false == Result)
		{
			mDesc = {};
			Result = false;
		}
		
		return Result;
	}

	HRESULT Texture::Load(const std::filesystem::path& _FileName)
	{
		stdfs::path FullPath = PathMgr::GetInst()->GetRelativeResourcePath(GetResType());

		HRESULT hr = LoadFile(FullPath / _FileName);

		if (FAILED(hr))
			return E_FAIL;

		InitializeResource();
		return S_OK;
	}

	HRESULT Texture::LoadFile(const std::filesystem::path& _fullPath)
	{
		std::wstring Extension = _fullPath.extension().wstring();
		StringConv::UpperCase(Extension);

		if (Extension == L".DDS")
		{
			if (FAILED(LoadFromDDSFile(_fullPath.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, nullptr, mImage)))
				return E_FAIL;
		}
		else if (Extension == L".TGA")
		{
			if (FAILED(LoadFromTGAFile(_fullPath.c_str(), nullptr, mImage)))
				return E_FAIL;
		}
		else // WIC (png, jpg, jpeg, bmp )
		{
			if (FAILED(LoadFromWICFile(_fullPath.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, nullptr, mImage)))
				return E_FAIL;
		}

		return S_OK;
	}

	void Texture::InitializeResource()
	{
		CreateShaderResourceView
		(
			GPUMgr::GetInst()->GetDevice().Get(),
			mImage.GetImages(),
			mImage.GetImageCount(),
			mImage.GetMetadata(),
			mSRV.GetAddressOf()
		);

		mSRV->GetResource((ID3D11Resource**)mTexture.GetAddressOf());
		mTexture->GetDesc(&mDesc);
	}

	void Texture::BindDataSRV(UINT _SRVSlot, eShaderStageFlag_ _stageFlag)
	{
		UnBind();
	
		mCurBoundRegister = (int)_SRVSlot;
		mCurBoundStage = _stageFlag;
		mCurBoundView = eBufferViewType::SRV;

		auto pContext = GPUMgr::GetInst()->GetContext();
		if (eShaderStageFlag::VS & _stageFlag)
		{
			pContext->VSSetShaderResources(_SRVSlot, 1u, mSRV.GetAddressOf());
		}
		if (eShaderStageFlag::HS & _stageFlag)
		{
			pContext->HSSetShaderResources(_SRVSlot, 1u, mSRV.GetAddressOf());
		}
		if (eShaderStageFlag::DS & _stageFlag)
		{
			pContext->DSSetShaderResources(_SRVSlot, 1u, mSRV.GetAddressOf());
		}
		if (eShaderStageFlag::GS & _stageFlag)
		{
			pContext->GSSetShaderResources(_SRVSlot, 1u, mSRV.GetAddressOf());
		}
		if (eShaderStageFlag::PS & _stageFlag)
		{
			pContext->PSSetShaderResources(_SRVSlot, 1u, mSRV.GetAddressOf());
		}
	}

	void Texture::BindDataUAV(UINT _startSlot)
	{
		UnBind();

		mCurBoundView = eBufferViewType::UAV;
		mCurBoundRegister = (int)_startSlot;

		UINT i = -1;
		GPUMgr::GetInst()->GetContext()->CSSetUnorderedAccessViews(_startSlot, 1, mUAV.GetAddressOf(), &i);
	}


	void Texture::UnBind()
	{
		switch (mCurBoundView)
		{
		case mh::eBufferViewType::NONE:
			break;

		case mh::eBufferViewType::SRV:
		{
			MH_ASSERT(0 <= mCurBoundRegister);
			ID3D11ShaderResourceView* srv = nullptr;

			auto pContext = GPUMgr::GetInst()->GetContext();

			if (eShaderStageFlag::VS & mCurBoundStage)
			{
				pContext->VSSetShaderResources(mCurBoundRegister, 1u, &srv);
			}
			if (eShaderStageFlag::HS & mCurBoundStage)
			{
				pContext->HSSetShaderResources(mCurBoundRegister, 1u, &srv);
			}
			if (eShaderStageFlag::DS & mCurBoundStage)
			{
				pContext->DSSetShaderResources(mCurBoundRegister, 1u, &srv);
			}
			if (eShaderStageFlag::GS & mCurBoundStage)
			{
				pContext->GSSetShaderResources(mCurBoundRegister, 1u, &srv);
			}
			if (eShaderStageFlag::PS & mCurBoundStage)
			{
				pContext->PSSetShaderResources(mCurBoundRegister, 1u, &srv);
			}
			if (eShaderStageFlag::CS & mCurBoundStage)
			{
				pContext->CSSetShaderResources(mCurBoundRegister, 1u, &srv);
			}

			mCurBoundRegister = -1;
			mCurBoundStage = eShaderStageFlag::NONE;
			
			break;
		}
		case mh::eBufferViewType::UAV:
		{
			ID3D11UnorderedAccessView* pUAV = nullptr;
			UINT u = -1;

			GPUMgr::GetInst()->GetContext()->CSSetUnorderedAccessViews(mCurBoundRegister, 1, &pUAV, &u);

			//현재 연결된 레지스터 번호와 파이프라인을 초기화
			mCurBoundRegister = -1;
			mCurBoundStage = eShaderStageFlag::NONE;

			break;
		}
			
		case mh::eBufferViewType::RTV:
			[[fallthrough]];
		case mh::eBufferViewType::DSV:
		{
			ID3D11RenderTargetView* pRTV = nullptr;
			ID3D11DepthStencilView* pDSV = nullptr;

			GPUMgr::GetInst()->GetContext()->OMSetRenderTargets(1u, &pRTV, pDSV);
			break;
		}

		default:
			break;
		}


		mCurBoundView = eBufferViewType::NONE;
	}

	bool Texture::CreateView()
	{
		auto pDevice = GPUMgr::GetInst()->GetDevice();
		if (mDesc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL)
		{
			if (FAILED(pDevice->CreateDepthStencilView(mTexture.Get(), nullptr, mDSV.GetAddressOf())))
			{
				ERROR_MESSAGE_W(L"Depth Stencil View 생성에 실패 했습니다.");
				return false;
			}
		}

		if (mDesc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET)
		{
			if (FAILED(pDevice->CreateRenderTargetView(mTexture.Get(), nullptr, mRTV.GetAddressOf())))
			{
				ERROR_MESSAGE_W(L"Render Target View 생성에 실패 했습니다.");
				return false;
			}
		}

		if (mDesc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};
			tSRVDesc.Format = mDesc.Format;
			tSRVDesc.Texture2D.MipLevels = 1;
			tSRVDesc.Texture2D.MostDetailedMip = 0;
			tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;

			if (FAILED(pDevice->CreateShaderResourceView(mTexture.Get(), nullptr, mSRV.GetAddressOf())))
			{
				ERROR_MESSAGE_W(L"Shader Resource View 생성에 실패 했습니다.");
				return false;
			}
		}

		if (mDesc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc = {};
			tUAVDesc.Format = mDesc.Format;
			tUAVDesc.Texture2D.MipSlice = 0;
			tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;

			if (FAILED(pDevice->CreateUnorderedAccessView(mTexture.Get(), nullptr, mUAV.GetAddressOf())))
			{
				ERROR_MESSAGE_W(L"Unordered Access View 생성에 실패 했습니다.");
				return false;
			}
		}

		return true;
	}


}
