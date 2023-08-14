#pragma once
#include "IAnimator.h"

#include "define_Struct.h"

#include <fbxsdk/fbxsdk.h>

namespace mh
{
	class StructBuffer;
	class Mesh;
	class Com_Animator3D :
		public IAnimator
	{
	public:
		Com_Animator3D();

		Com_Animator3D(const Com_Animator3D& _other);
		CLONE(Com_Animator3D);

		virtual ~Com_Animator3D();

		virtual void FixedUpdate() override;

		void SetBones(const std::vector<define::tMTBone>* _vecBones) { m_pVecBones = _vecBones; m_vecFinalBoneMat.resize(m_pVecBones->size()); }
		void SetAnimClip(const std::vector<define::tMTAnimClip>* _vecAnimClip);
		void SetClipTime(int _iClipIdx, float _fTime) { m_vecClipUpdateTime[_iClipIdx] = _fTime; }

		StructBuffer* GetFinalBoneMat() { return m_pBoneFinalMatBuffer; }
		UINT GetBoneCount() { return (UINT)m_pVecBones->size(); }

		virtual void Binds();
		virtual void Clear();

		virtual void Init() {};
		virtual void Update() {};
		virtual void Render() {};

	private:
		void check_mesh(std::shared_ptr<Mesh> _pMesh);

    private:
        const std::vector<define::tMTBone>* m_pVecBones;
        const std::vector<define::tMTAnimClip>* m_pVecClip;

        std::vector<float>				m_vecClipUpdateTime;
        std::vector<MATRIX>				m_vecFinalBoneMat; // 텍스쳐에 전달할 최종 행렬정보
        int							m_iFrameCount; // 30
        double						m_dCurTime;
        int							m_iCurClip; // 클립 인덱스	

        int							m_iFrameIdx; // 클립의 현재 프레임
        int							m_iNextFrameIdx; // 클립의 다음 프레임
        float						m_fRatio;	// 프레임 사이 비율

        StructBuffer* m_pBoneFinalMatBuffer;  // 특정 프레임의 최종 행렬
        bool						m_bFinalMatUpdate; // 최종행렬 연산 수행여부
	};
}


